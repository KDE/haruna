/*
 * SPDX-FileCopyrightText: 2025 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "youtube.h"

#include <QJsonDocument>
#include <QJsonValue>
#include <QProcess>
#include <QQmlEngine>
#include <QStandardPaths>
#include <QUrlQuery>

#include <KLocalizedString>

#include "application.h"

using namespace Qt::StringLiterals;

YouTube::YouTube()
{
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

    auto youtubeDl = QStandardPaths::findExecutable(u"youtube-dl"_s);
    if (!youtubeDl.isEmpty()) {
        return youtubeDl;
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
    QString playlistPath = u"https://www.youtube.com/playlist?list=%1"_s.arg(playlistId);

    // use youtube-dl to get the required playlist info as json
    auto ytdlProcess = std::make_shared<QProcess>();
    auto args = QStringList() << u"-J"_s << u"--flat-playlist"_s << playlistId;
    ytdlProcess->setProgram(youtubeDlExecutable());
    ytdlProcess->setArguments(args);
    ytdlProcess->start();

    connect(ytdlProcess.get(), QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [=](int, QProcess::ExitStatus) {
        const auto output = ytdlProcess->readAllStandardOutput();
        const auto error = ytdlProcess->readAllStandardError();
        const QString json = QString::fromUtf8(output);
        const QJsonValue entries = QJsonDocument::fromJson(json.toUtf8())[u"entries"_s];
        // QString playlistTitle = QJsonDocument::fromJson(json.toUtf8())[u"title")].toString();
        const auto playlist = entries.toArray();

        if (output.contains("null\n")) {
            Application::instance()->error(i18nc("@info:tooltip; error when yt-dlp fails to get playlist",
                                                 "Could not retrieve playlist with id: %1\n\n%2",
                                                 playlistId,
                                                 QString::fromUtf8(error)));
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

void YouTube::getVideoInfo(const QUrl &url, QVariantMap data)
{
    QUrlQuery query{url.query()};
    QString playlistId{query.queryItemValue(u"list"_s)};
    QString videoId{query.queryItemValue(u"v"_s)};

    QUrl urlWithoutPlaylist = url;
    if (!playlistId.isEmpty()) {
        urlWithoutPlaylist = QUrl{u"https://www.youtube.com/watch?v=%1"_s.arg(videoId)};
    }

    auto ytdlProcess = std::make_shared<QProcess>();
    ytdlProcess->setProgram(youtubeDlExecutable());
    ytdlProcess->setArguments(QStringList() << u"-j"_s << urlWithoutPlaylist.toString());
    ytdlProcess->start();

    connect(ytdlProcess.get(), QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [=](int, QProcess::ExitStatus) {
        QString json = QString::fromUtf8(ytdlProcess->readAllStandardOutput());
        QString title = QJsonDocument::fromJson(json.toUtf8())[u"title"_s].toString();
        auto duration = QJsonDocument::fromJson(json.toUtf8())[u"duration"_s].toDouble();
        YTVideoInfo info;
        info.url = url;
        info.mediaTitle = title;
        info.duration = duration;
        Q_EMIT videoInfoRetrieved(info, data);
    });
}
