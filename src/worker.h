/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <KFileMetaData/Properties>

class Worker : public QObject
{
    Q_OBJECT
public:
    static Worker* instance();

signals:
    void metaDataReady(int index, KFileMetaData::PropertyMap metadata);
    void thumbnailSuccess(const QImage &image);
    void thumbnailFail();

public slots:
    void getMetaData(int index, const QString &path);
    void makePlaylistThumbnail(const QString &id, int width);

private:
    Worker() = default;
    ~Worker() = default;

    Worker(const Worker &) = delete;
    Worker &operator=(const Worker &) = delete;
    Worker(Worker &&) = delete;
    Worker &operator=(Worker &&) = delete;
};

#endif // WORKER_H
