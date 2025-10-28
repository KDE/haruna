/*
 * SPDX-FileCopyrightText: 2025 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QtQml/qqmlregistration.h>

class QQmlEngine;
class QJSEngine;
struct RecentFile;

class Database : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    static Database *instance();
    static Database *create(QQmlEngine *, QJSEngine *);

    QSqlDatabase db();

    QList<RecentFile> recentFiles(uint limit);
    void addRecentFile(const QUrl &url, const QString &filename, const QString &openedFrom, qint64 timestamp);
    void deleteRecentFiles();

    double playbackPosition(const QString &md5Hash);
    void addPlaybackPosition(const QString &md5Hash, const QString &path, double position, QSqlDatabase dbConnection = QSqlDatabase{});
    void deletePlaybackPositions();
    void deletePlaybackPosition(const QString &md5Hash);

private:
    Database(QObject *parent = nullptr);
    void createTables();

    Q_DISABLE_COPY_MOVE(Database)
};

#endif // DATABASE_H
