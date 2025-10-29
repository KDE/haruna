/*
 * SPDX-FileCopyrightText: 2025 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "database.h"

#include <QFile>
#include <QQmlEngine>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStringBuilder>

#include "pathutils.h"
#include "recentfile.h"

using namespace Qt::StringLiterals;

const QString RECENT_FILES_TABLE = u"recent_files"_s;
const QString PLAYBACK_POSITION_TABLE = u"playback_position"_s;

QString getLastExecutedQuery(const QSqlQuery &query)
{
    QString str = query.lastQuery();

    const QVariantList values = query.boundValues();
    const QStringList keys = query.boundValueNames();

    for (qsizetype i = 0; i < values.size(); ++i) {
        str.replace(keys.at(i), values.at(i).toString());
    }

    return str;
}

Database *Database::instance()
{
    static Database *db = new Database();
    return db;
}

Database *Database::create(QQmlEngine *, QJSEngine *)
{
    QQmlEngine::setObjectOwnership(instance(), QQmlEngine::CppOwnership);
    return instance();
}

Database::Database(QObject *parent)
    : QObject(parent)
{
    const auto dbFile{PathUtils::instance()->configFilePath(PathUtils::ConfigFile::Database)};

    auto mangaDB = QSqlDatabase::addDatabase(u"QSQLITE"_s, u"haruna"_s);
    mangaDB.setDatabaseName(dbFile);
    if (mangaDB.open()) {
        createTables();
    } else {
        qDebug() << "Could not open database:" << mangaDB.lastError();
    }
}

void Database::createTables()
{
    QStringList tables = db().tables();

    if (!tables.contains(u"recent_files"_s)) {
        QFile sqlFile(u":sql/create-recent_files-table.sql"_s);
        if (sqlFile.open(QFile::ReadOnly)) {
            QSqlQuery qManga(db());
            qManga.exec(QString::fromUtf8(sqlFile.readAll()));
        }
    }
    if (!tables.contains(u"playback_position"_s)) {
        QFile sqlFile(u":sql/create-playback_position-table.sql"_s);
        if (sqlFile.open(QFile::ReadOnly)) {
            QSqlQuery qManga(db());
            qManga.exec(QString::fromUtf8(sqlFile.readAll()));
        }
    }
}

QSqlDatabase Database::db()
{
    return QSqlDatabase::database(u"haruna"_s);
}

QList<RecentFile> Database::recentFiles(uint limit)
{
    QSqlQuery query(db());
    query.prepare(u"SELECT * FROM "_s % RECENT_FILES_TABLE % u" ORDER BY timestamp DESC LIMIT "_s % QString::number(limit));
    query.exec();

    QList<RecentFile> recentFiles;
    while (query.next()) {
        RecentFile rf;
        rf.id = query.value(u"recent_file_id"_s).toUInt();
        rf.url = query.value(u"url"_s).toUrl();
        rf.filename = query.value(u"filename"_s).toString();
        rf.openedFrom = query.value(u"opened_from"_s).toString();
        rf.timestamp = query.value(u"timestamp"_s).toLongLong();

        recentFiles.append(rf);
    }

    if (query.lastError().isValid()) {
        qDebug() << query.lastError() << getLastExecutedQuery(query);
    }

    return recentFiles;
}

void Database::addRecentFile(const QUrl &url, const QString &filename, const QString &openedFrom, qint64 timestamp)
{
    QSqlQuery query(db());
    query.prepare(u"INSERT INTO "_s % RECENT_FILES_TABLE %
                  u" (url, filename, opened_from, timestamp) "
                  "VALUES (:url, :filename, :openedFrom, :timestamp) "
                  "ON CONFLICT(url) DO UPDATE SET "
                  "opened_from = :openedFrom, timestamp = :timestamp"_s);
    query.bindValue(u":url"_s, url);
    query.bindValue(u":filename"_s, filename);
    query.bindValue(u":openedFrom"_s, openedFrom);
    query.bindValue(u":timestamp"_s, timestamp);
    query.exec();

    if (query.lastError().isValid()) {
        qDebug() << query.lastError() << getLastExecutedQuery(query);
    }
}

void Database::deleteRecentFiles()
{
    QSqlQuery query(db());
    query.exec(u"DELETE FROM "_s % RECENT_FILES_TABLE);
}

double Database::playbackPosition(const QString &md5Hash)
{
    QSqlQuery query(db());
    query.prepare(u"SELECT * FROM "_s % PLAYBACK_POSITION_TABLE % u" WHERE md5_hash = :md5Hash LIMIT 1"_s);
    query.bindValue(u":md5Hash"_s, md5Hash);
    query.exec();

    if (query.lastError().isValid()) {
        qDebug() << query.lastError() << getLastExecutedQuery(query);
    }

    while (query.first()) {
        return query.value(u"position"_s).toDouble();
    }

    return 0.0;
}

void Database::addPlaybackPosition(const QString &md5Hash, const QString &path, double position, QSqlDatabase dbConnection)
{
    QSqlDatabase database = dbConnection.isValid() ? dbConnection : db();
    QSqlQuery query(database);
    query.prepare(u"INSERT INTO "_s % PLAYBACK_POSITION_TABLE %
                  u" (md5_hash, path, position) "
                  "VALUES (:md5Hash, :path, :position) "
                  "ON CONFLICT(md5_hash) DO UPDATE SET "
                  "position = excluded.position"_s);
    query.bindValue(u":md5Hash"_s, md5Hash);
    query.bindValue(u":path"_s, path);
    query.bindValue(u":position"_s, position);
    query.exec();

    if (query.lastError().isValid()) {
        qDebug() << query.lastError() << getLastExecutedQuery(query);
    }
}

void Database::deletePlaybackPositions()
{
    QSqlQuery query(db());
    query.exec(u"DELETE FROM "_s % PLAYBACK_POSITION_TABLE);
}

void Database::deletePlaybackPosition(const QString &md5Hash)
{
    QSqlQuery query(db());
    query.prepare(u"DELETE FROM "_s % PLAYBACK_POSITION_TABLE %
                  u" WHERE md5_hash = :md5Hash"_s);
    query.bindValue(u":md5Hash"_s, md5Hash);
    query.exec();

    if (query.lastError().isValid()) {
        qDebug() << query.lastError() << getLastExecutedQuery(query);
    }
}

#include "moc_database.cpp"
