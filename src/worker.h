/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QImage>
#include <KFileMetaData/Properties>
#include <memory>
#include <kconfig_version.h>

class KConfig;

class Worker : public QObject
{
    Q_OBJECT
public:
    static Worker* instance();

Q_SIGNALS:
#if KCONFIG_VERSION >= QT_VERSION_CHECK(5, 89, 0)
    void metaDataReady(int index, KFileMetaData::PropertyMultiMap metadata);
#else
    void metaDataReady(int index, KFileMetaData::PropertyMap metadata);
#endif
    void thumbnailSuccess(const QString &id, const QImage &image);
    void thumbnailFail();

public Q_SLOTS:
    void getMetaData(int index, const QString &path);
    void makePlaylistThumbnail(const QString &id, int width);
    QImage frameToImage(const QString &id, int width);
    void syncConfigValue(QString path, QString group, QString key, QVariant value);

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
