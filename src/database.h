/*
 * SPDX-FileCopyrightText: 2025 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QUrl>
#include <QtQml/qqmlregistration.h>

#include <KFileMetaData/Properties>

class QQmlEngine;
class QJSEngine;
struct RecentFile;

struct CachedMetadata {
    KFileMetaData::PropertyMultiMap properties;
    QUrl url;
    QString formattedDuration;
    qint64 metadataId;
};

class Database : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    static Database *instance();
    static Database *create(QQmlEngine *, QJSEngine *);

    QSqlDatabase connection();

    QList<RecentFile> recentFiles(uint limit);
    void addRecentFile(const QUrl &url, const QString &filename, const QString &openedFrom, qint64 timestamp);
    void deleteRecentFiles();

    double playbackPosition(const QString &md5Hash);
    void addPlaybackPosition(const QString &md5Hash, const QString &path, double position, QSqlDatabase dbConnection = QSqlDatabase{});
    Q_INVOKABLE void deletePlaybackPositions();
    void deletePlaybackPosition(const QString &md5Hash);

    int insertMetadata(const QUrl &url, const KFileMetaData::PropertyMultiMap &properties);
    Q_INVOKABLE bool updateMetadata(const QUrl &url);
    Q_INVOKABLE CachedMetadata getMetadata(const QUrl &url);
    Q_INVOKABLE bool deleteMetadata(const QUrl &url);
    Q_INVOKABLE bool deleteAllMetadata();

private:
    Database(QObject *parent = nullptr);
    void createTables();
    void createTable(const QString &filename);

    Q_DISABLE_COPY_MOVE(Database)
};

#endif // DATABASE_H
