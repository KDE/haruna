/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QSqlDatabase>

#include <KFileMetaData/Properties>

#include <memory>

class KConfig;
class QImage;
class QQuickWindow;

class Worker : public QObject
{
    Q_OBJECT
public:
    static Worker *instance();

Q_SIGNALS:
    void metaDataReady(uint index, const QUrl &url, KFileMetaData::PropertyMultiMap metadata);
    void thumbnailSuccess(const QString &path, const QImage &image);
    void thumbnailFail();
    void mprisThumbnailSuccess(const QImage &image);
    void subtitlesFound(QStringList subs);
    void ytdlpVersionRetrived(const QByteArray &version);

public Q_SLOTS:
    void getMetaData(uint index, const QString &path);
    void makePlaylistThumbnail(const QString &path, int width);
    QImage frameToImage(const QString &path, int width);
    void savePositionToDB(const QString &md5Hash, const QString &path, double position);
    void mprisThumbnail(const QString &path, int width);
    void findRecursiveSubtitles(const QUrl &url);
    void getYtdlpVersion();

private:
    Worker() = default;
    ~Worker() = default;

    Worker(const Worker &) = delete;
    Worker &operator=(const Worker &) = delete;
    Worker(Worker &&) = delete;
    Worker &operator=(Worker &&) = delete;

    QSqlDatabase getDBConnection();
};

#endif // WORKER_H
