/*
 * SPDX-FileCopyrightText: 2025 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "youtube.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QProcess>
#include <QQmlEngine>
#include <QStandardPaths>
#include <QUrlQuery>

#include <KLocalizedString>

using namespace Qt::StringLiterals;

YouTube::YouTube() = default;

YouTube::~YouTube()
{
    cancelYtDlpProcesses();
}

bool YouTube::hasYoutubeDl()
{
    return !youtubeDlExecutable().isEmpty();
}

QString YouTube::youtubeDlExecutable()
{
    auto ytDlp = QStandardPaths::findExecutable(u"yt-dlp"_s);
    if (!ytDlp.isEmpty()) {
        return ytDlp;
    }

    return QString{};
}

bool YouTube::isPlaylist(const QUrl &url)
{
    if (!isYoutubeUrl(url)) {
        return false;
    }

    QUrlQuery query{url.query()};
    return query.hasQueryItem(u"list"_s);
}

void YouTube::getPlaylist(const QUrl &url)
{
    QUrlQuery query{url.query()};
    QString playlistId{query.queryItemValue(u"list"_s)};
    QString videoId{query.queryItemValue(u"v"_s)};

    // use youtube-dl to get the required playlist info as json
    auto ytdlProcess = std::make_shared<QProcess>();
    auto args = QStringList() << u"-J"_s << u"--flat-playlist"_s << playlistId;
    ytdlProcess->setProgram(youtubeDlExecutable());
    ytdlProcess->setArguments(args);
    ytdlProcess->start();

    connect(ytdlProcess.get(),
            QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this,
            [this, ytdlProcess, playlistId, videoId](int, QProcess::ExitStatus) {
                const auto output = ytdlProcess->readAllStandardOutput();
                const auto errorString = ytdlProcess->readAllStandardError();
                const QString json = QString::fromUtf8(output);
                const auto jsonObject = QJsonDocument::fromJson(json.toUtf8()).object();
                const QJsonValue entries = jsonObject.value(u"entries");
                // QString playlistTitle = jsonObject.value(u"title").toString();
                const auto playlist = entries.toArray();

                if (output.contains("null\n")) {
                    Q_EMIT error(i18nc("@info:tooltip; error when yt-dlp fails to get playlist",
                                       "Could not retrieve playlist with id: %1\n\n%2",
                                       playlistId,
                                       QString::fromUtf8(errorString)));
                }

                if (playlist.isEmpty()) {
                    return;
                }

                Q_EMIT playlistRetrieved(playlist, videoId, playlistId);
            });
}

QUrl YouTube::normalizeUrl(const QUrl &url)
{
    if (!isYoutubeUrl(url)) {
        return url;
    }

    QUrlQuery query{url.query()};
    QString playlistId{query.queryItemValue(u"list"_s)};
    QString videoId{query.queryItemValue(u"v"_s)};

    if (videoId.isEmpty() && url.host().contains(u"youtu.be"_s)) {
        videoId = url.fileName();
    }

    QUrl normalizedUrl;
    if (playlistId.isEmpty()) {
        normalizedUrl = QUrl{u"https://www.youtube.com/watch?v=%1"_s.arg(videoId)};
    } else {
        normalizedUrl = QUrl{u"https://www.youtube.com/watch?v=%1&list=%2"_s.arg(videoId, playlistId)};
    }

    return normalizedUrl;
}

bool YouTube::isYoutubeUrl(const QUrl &url)
{
    if (url.host().contains(u"youtube.com"_s) || url.host().contains(u"youtu.be"_s)) {
        return true;
    }
    return false;
}

void YouTube::getVideoInfo(const QUrl &url, const QVariantMap &data)
{
    QUrlQuery query{url.query()};
    QString videoId{query.queryItemValue(u"v"_s)};

    QStringList arguments{
        u"--skip-download"_s,
        u"--print"_s,
        u"id:%(id)s duration:%(duration)s title:%(title)s"_s,
        u"--"_s,
    };
    arguments.append(videoId);

    auto ytdlProcess = std::make_shared<QProcess>();
    ytdlProcess->setProgram(youtubeDlExecutable());
    ytdlProcess->setArguments(arguments);

    connect(ytdlProcess.get(), QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [this, ytdlProcess, url, data](int, QProcess::ExitStatus) {
        const auto output = QString::fromUtf8(ytdlProcess->readAllStandardOutput());
        const auto info = stringToYTVideoInfo(output);
        if (info.has_value()) {
            Q_EMIT videoInfoRetrieved(info.value(), data);
        }
    });
    ytdlProcess->start();
}

void YouTube::getVideoInfo(YTRequestPayload payload)
{
    int i = 0;
    int batchSize = 5;
    QList<QStringList> youtubeIdsList;
    QStringList currentBatch;

    for (auto [videoId, row] : payload.pairs.asKeyValueRange()) {
        currentBatch.append(videoId);
        if (currentBatch.size() == batchSize) {
            youtubeIdsList.append(currentBatch);
            currentBatch.clear();
            if (i == 2) {
                batchSize = 10;
            }
            if (i == 5) {
                batchSize = 20;
            }
            if (i == 10) {
                batchSize = 30;
            }
            i++;
        }
    }
    if (!currentBatch.isEmpty()) {
        youtubeIdsList.append(currentBatch);
    }

    m_pendingYoutubeBatches.clear();

    for (const auto &batch : std::as_const(youtubeIdsList)) {
        m_pendingYoutubeBatches.enqueue(batch);
    }

    startNextYtDlpProcess(payload);
}

void YouTube::startNextYtDlpProcess(YTRequestPayload payload)
{
    while (m_runningYtDlpProcesses.size() < MaxConcurrentProcesses && !m_pendingYoutubeBatches.isEmpty()) {
        const QStringList youtubeIds = m_pendingYoutubeBatches.dequeue();

        QStringList arguments{
            u"--skip-download"_s,
            u"--print"_s,
            u"id:%(id)s duration:%(duration)s title:%(title)s"_s,
            u"--"_s,
        };
        arguments.append(youtubeIds);

        auto ytdlProcess = std::make_shared<QProcess>();
        ytdlProcess->setProgram(youtubeDlExecutable());
        ytdlProcess->setArguments(arguments);

        m_runningYtDlpProcesses.append(ytdlProcess);

        const auto onYtDlpFinished = [this, ytdlProcess, youtubeIds, payload](int exitCode, QProcess::ExitStatus status) {
            if (status != QProcess::NormalExit || exitCode != 0) {
                qWarning() << "yt-dlp failed:" << exitCode << ytdlProcess->errorString();
            }

            const auto output = QString::fromUtf8(ytdlProcess->readAllStandardOutput());
            const auto data = output.split(u'\n');

            YTResponsePayload responsePayload;
            responsePayload.playlistVersion = payload.playlistVersion;

            for (const auto &string : data) {
                const auto info = stringToYTVideoInfo(string);
                if (info.has_value()) {
                    const auto value = info.value();
                    responsePayload.pairs.insert(payload.pairs.value(value.videoId), value);
                }
            }

            Q_EMIT videosInfoRetrieved(responsePayload);

            m_runningYtDlpProcesses.removeAll(ytdlProcess);
            startNextYtDlpProcess(payload);
        };

        connect(ytdlProcess.get(), QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, onYtDlpFinished);

        ytdlProcess->start();
    }
}

std::optional<YTVideoInfo> YouTube::stringToYTVideoInfo(const QString &string)
{
    if (string.isEmpty()) {
        return std::nullopt;
    }

    QStringView line(string);
    const qsizetype idStart = line.indexOf(u"id:") + 3;
    const qsizetype durationStart = line.indexOf(u" duration:");
    const qsizetype titleStart = line.indexOf(u" title:");
    const QString id = line.mid(idStart, durationStart - idStart).toString();

    YTVideoInfo info;
    info.url = QUrl{u"https://www.youtube.com/watch?v=%1"_s.arg(id)};
    info.videoId = id;
    info.mediaTitle = line.mid(titleStart + 7).toString();
    info.duration = line.mid(durationStart + 10, titleStart - durationStart - 10).toDouble();

    return {info};
}

void YouTube::cancelYtDlpProcesses()
{
    m_pendingYoutubeBatches.clear();

    const auto processes = m_runningYtDlpProcesses;
    m_runningYtDlpProcesses.clear();

    for (const auto &process : processes) {
        if (process->state() != QProcess::NotRunning) {
            process->kill();
            process->waitForFinished(1000);
        }
    }
}