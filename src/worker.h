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

public slots:
    void getMetaData(int index, const QString &path);

private:
    Q_DISABLE_COPY_MOVE(Worker)
    Worker() = default;
    ~Worker() = default;
};

#endif // WORKER_H
