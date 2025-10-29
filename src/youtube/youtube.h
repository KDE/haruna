/*
 * SPDX-FileCopyrightText: 2025 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef YOUTUBE_H
#define YOUTUBE_H

#include <QJsonArray>
#include <QObject>
#include <qqmlintegration.h>

class QQmlEngine;
class QJSEngine;

struct YTVideoInfo {
    QUrl url;
    QString mediaTitle;
    double duration;
};

class YouTube : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit YouTube();

    Q_INVOKABLE bool hasYoutubeDl();
    Q_INVOKABLE QString youtubeDlExecutable();
    Q_INVOKABLE bool isPlaylist(const QUrl &url);
    Q_INVOKABLE void getPlaylist(const QUrl &url);
    Q_INVOKABLE QUrl normalizeUrl(const QUrl &url);
    Q_INVOKABLE bool isYoutubeUrl(const QUrl &url);
    Q_INVOKABLE void getVideoInfo(const QUrl &url, QVariantMap data);

Q_SIGNALS:
    void playlistRetrieved(QJsonArray playlist, const QString &videoId, const QString &playlistId);
    void videoInfoRetrieved(YTVideoInfo info, QVariantMap data);
    void error(const QString &message);

private:
};

#endif // YOUTUBE_H
