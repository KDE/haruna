/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef WORKER_H
#define WORKER_H

#include <QImage>
#include <QObject>

#include <KFileMetaData/Properties>
#include <kconfig_version.h>

#include <memory>

class KConfig;
class QQuickWindow;

class Worker : public QObject
{
    Q_OBJECT
public:
    static Worker *instance();

Q_SIGNALS:
    void metaDataReady(uint index, KFileMetaData::PropertyMultiMap metadata);
    void thumbnailSuccess(const QString &path, const QImage &image);
    void thumbnailFail();
    void mprisThumbnailSuccess(const QImage &image);
    void subtitlesFound(QStringList subs);

public Q_SLOTS:
    void getMetaData(uint index, const QString &path);
    void makePlaylistThumbnail(const QString &path, int width);
    QImage frameToImage(const QString &path, int width);
    void syncConfigValue(QString path, QString group, QString key, QVariant value);
    void saveWindowGeometry(QQuickWindow *window) const;
    void mprisThumbnail(const QString &path, int width);
    void findRecursiveSubtitles(const QUrl &url);

private:
    Worker() = default;
    ~Worker() = default;

    Worker(const Worker &) = delete;
    Worker &operator=(const Worker &) = delete;
    Worker(Worker &&) = delete;
    Worker &operator=(Worker &&) = delete;

private:
    std::unique_ptr<KConfig> m_cachedConf;
};

#endif // WORKER_H
