/*
 * SPDX-FileCopyrightText: 2025 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef YOUTUBE_H
#define YOUTUBE_H

#include <QJsonArray>
#include <QObject>
#include <QProcess>
#include <QQueue>
#include <qqmlintegration.h>

class QQmlEngine;
class QJSEngine;

struct YTVideoInfo {
    QUrl url;
    QString videoId;
    QString mediaTitle;
    double duration{0.0};
};

using YouTubeIdRowPairs = QHash<QString, int>;
using RowYouTubeInfoPairs = QHash<int, YTVideoInfo>;

struct YTRequestPayload {
    int playlistVersion{0};
    YouTubeIdRowPairs pairs;
};

struct YTResponsePayload {
    int playlistVersion{0};
    RowYouTubeInfoPairs pairs;
};

class YouTube : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit YouTube();
    ~YouTube();

    Q_INVOKABLE bool hasYoutubeDl();
    Q_INVOKABLE QString youtubeDlExecutable();
    Q_INVOKABLE bool isPlaylist(const QUrl &url);
    Q_INVOKABLE void getPlaylist(const QUrl &url);
    Q_INVOKABLE QUrl normalizeUrl(const QUrl &url);
    Q_INVOKABLE bool isYoutubeUrl(const QUrl &url);
    Q_INVOKABLE void getVideoInfo(const QUrl &url, const QVariantMap &data);
    void getVideoInfo(YTRequestPayload payload);
    void cancelYtDlpProcesses();

Q_SIGNALS:
    void playlistRetrieved(QJsonArray playlist, const QString &videoId, const QString &playlistId);
    void videoInfoRetrieved(YTVideoInfo info, QVariantMap data);
    void videosInfoRetrieved(YTResponsePayload payload);
    void error(const QString &message);

private:
    void startNextYtDlpProcess(YTRequestPayload payload);
    std::optional<YTVideoInfo> stringToYTVideoInfo(const QString &string);
    QQueue<QStringList> m_pendingYoutubeBatches;
    QList<std::shared_ptr<QProcess>> m_runningYtDlpProcesses;
    static constexpr int MaxConcurrentProcesses = 5;
};

#endif // YOUTUBE_H
