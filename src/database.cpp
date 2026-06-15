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
#include <QThread>

#include <KFileMetaData/ExtractorCollection>
#include <KFileMetaData/SimpleExtractionResult>

#include "miscutils.h"
#include "pathutils.h"
#include "recentfile.h"

using namespace Qt::StringLiterals;

const QString RECENT_FILES_TABLE = u"recent_files"_s;
const QString PLAYBACK_POSITION_TABLE = u"playback_position"_s;
const QString METADATA_CACHE_TABLE = u"metadata_cache"_s;

static const QString insertMetadataCacheSql = u"INSERT INTO " % METADATA_CACHE_TABLE % u" " %
    u"(url, title, duration, "
    u"width, height, aspect_ratio, framerate, video_codec, orientation, "
    u"track_no, disc_no, album, artist, album_artist, audio_codec, "
    "bitrate, sample_rate, genre, release_year, composer, lyricist) "
    u"VALUES "
    u"(:url, :title, :duration, "
    ":width, :height, :aspect_ratio, :framerate, :video_codec, :orientation, "
    ":track_no, :disc_no, :album, :artist, :album_artist, :audio_codec, "
    ":bitrate, :sample_rate, :genre, :release_year, :composer, :lyricist);";

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
    static Database db;
    return &db;
}

Database *Database::create(QQmlEngine *, QJSEngine *)
{
    QQmlEngine::setObjectOwnership(instance(), QQmlEngine::CppOwnership);
    return instance();
}

Database::Database(QObject *parent)
    : QObject(parent)
{
    if (connection().open()) {
        createTables();
    } else {
        qDebug() << "Could not open database:" << connection().lastError();
    }
}

QSqlDatabase Database::connection()
{
    const QString connName = u"thread_connection_%1"_s.arg((quintptr)QThread::currentThreadId());

    if (QSqlDatabase::contains(connName)) {
        return QSqlDatabase::database(connName);
    }

    const auto dbFile{PathUtils::instance()->configFilePath(PathUtils::ConfigFile::Database)};
    QSqlDatabase db = QSqlDatabase::addDatabase(u"QSQLITE"_s, connName);
    db.setDatabaseName(dbFile);

    if (!db.open()) {
        qWarning() << "Failed to open DB in thread" << QThread::currentThread() << db.lastError();
    }

    QSqlQuery query(db);
    if (!query.exec(u"PRAGMA foreign_keys = '1';"_s)) {
        qDebug() << "Failed to enable foreign keys:" << query.lastError().text();
    }
    if (!query.exec(u"PRAGMA journal_mode=WAL;"_s)) {
        qDebug() << "Failed to enable WAL mode:" << query.lastError().text();
    }

    return db;
}

void Database::createTables()
{
    QStringList tables = connection().tables();

    if (!tables.contains(u"metadata_cache"_s)) {
        createTable(u":sql/create-metadata_cache-table.sql"_s);
    }
    if (!tables.contains(u"recent_files"_s)) {
        createTable(u":sql/create-recent_files-table.sql"_s);
    }
    if (!tables.contains(u"playback_position"_s)) {
        createTable(u":sql/create-playback_position-table.sql"_s);
    }
}

void Database::createTable(const QString &filename)
{
    QFile sqlFile(filename);
    if (!sqlFile.open(QFile::ReadOnly)) {
        qDebug() << sqlFile.fileName() << sqlFile.errorString();
    }

    auto content = QString::fromUtf8(sqlFile.readAll());

    QSqlQuery query(connection());
    const auto result = query.exec(content);

    if (!result) {
        qDebug() << sqlFile.fileName() << query.lastError().text() << getLastExecutedQuery(query);
    }
}

QList<RecentFile> Database::recentFiles(uint limit)
{
    QSqlQuery query(connection());
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
    QSqlQuery query(connection());
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
    QSqlQuery query(connection());
    query.exec(u"DELETE FROM "_s % RECENT_FILES_TABLE);
}

double Database::playbackPosition(const QString &md5Hash)
{
    QSqlQuery query(connection());
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
    QSqlDatabase database = dbConnection.isValid() ? dbConnection : connection();
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
    QSqlQuery query(connection());
    query.exec(u"DELETE FROM "_s % PLAYBACK_POSITION_TABLE);
}

void Database::deletePlaybackPosition(const QString &md5Hash)
{
    QSqlQuery query(connection());
    query.prepare(u"DELETE FROM "_s % PLAYBACK_POSITION_TABLE %
                  u" WHERE md5_hash = :md5Hash"_s);
    query.bindValue(u":md5Hash"_s, md5Hash);
    query.exec();

    if (query.lastError().isValid()) {
        qDebug() << query.lastError() << getLastExecutedQuery(query);
    }
}

int Database::insertMetadata(const QUrl &url, const KFileMetaData::PropertyMultiMap &properties)
{
    QSqlQuery query(connection());
    query.prepare(insertMetadataCacheSql);
    query.bindValue(u":url"_s, url);
    query.bindValue(u":title"_s, properties.value(KFileMetaData::Property::Title));
    query.bindValue(u":duration"_s, properties.value(KFileMetaData::Property::Duration));
    query.bindValue(u":width"_s, properties.value(KFileMetaData::Property::Width));
    query.bindValue(u":height"_s, properties.value(KFileMetaData::Property::Height));
    query.bindValue(u":aspect_ratio"_s, properties.value(KFileMetaData::Property::AspectRatio));
    query.bindValue(u":framerate"_s, properties.value(KFileMetaData::Property::FrameRate));
    query.bindValue(u":video_codec"_s, properties.value(KFileMetaData::Property::VideoCodec));
    query.bindValue(u":orientation"_s, properties.value(KFileMetaData::Property::ImageOrientation));
    query.bindValue(u":track_no"_s, properties.value(KFileMetaData::Property::TrackNumber));
    query.bindValue(u":disc_no"_s, properties.value(KFileMetaData::Property::DiscNumber));
    query.bindValue(u":album"_s, properties.value(KFileMetaData::Property::Album));
    query.bindValue(u":artist"_s, properties.value(KFileMetaData::Property::Artist));
    query.bindValue(u":album_artist"_s, properties.value(KFileMetaData::Property::AlbumArtist));
    query.bindValue(u":audio_codec"_s, properties.value(KFileMetaData::Property::AudioCodec));
    query.bindValue(u":bitrate"_s, properties.value(KFileMetaData::Property::BitRate));
    query.bindValue(u":sample_rate"_s, properties.value(KFileMetaData::Property::SampleRate));
    query.bindValue(u":genre"_s, properties.value(KFileMetaData::Property::Genre));
    query.bindValue(u":release_year"_s, properties.value(KFileMetaData::Property::ReleaseYear));
    query.bindValue(u":composer"_s, properties.value(KFileMetaData::Property::Composer));
    query.bindValue(u":lyricist"_s, properties.value(KFileMetaData::Property::Lyricist));

    const auto result = query.exec();
    if (!result) {
        qDebug() << "Database::insertMetadata:" << query.lastError().text() << getLastExecutedQuery(query);
        return 0;
    }

    return query.lastInsertId().toInt();
}

bool Database::updateMetadata(const QUrl &url)
{
    if (url.scheme() != u"file"_s) {
        return false;
    }

    QString mimeType = MiscUtils::mimeType(url);

    using namespace KFileMetaData;
    ExtractorCollection exCol;
    QList<Extractor *> extractors = exCol.fetchExtractors(mimeType);
    SimpleExtractionResult result(url.toLocalFile(), mimeType, ExtractionResult::ExtractMetaData);

    if (extractors.isEmpty()) {
        return false;
    }

    Extractor *ex = extractors.first();
    ex->extract(&result);

    if (!deleteMetadata(url)) {
        return false;
    }

    const auto lastInsertId = insertMetadata(url, result.properties());

    return lastInsertId > 0;
}

CachedMetadata Database::getMetadata(const QUrl &url)
{
    QSqlQuery query(connection());
    query.prepare(u"SELECT * FROM " % METADATA_CACHE_TABLE % u" WHERE url = :url");
    query.bindValue(u":url"_s, url);

    const auto result = query.exec();
    if (!result) {
        qDebug() << "Database::getMetadata:" << query.lastError().text() << getLastExecutedQuery(query);
        return {};
    }

    if (!query.first()) {
        // qDebug() << "Database::getMetadata: metadata not found for" << url;
        return {};
    }

    KFileMetaData::PropertyMultiMap properties;
    properties.insert(KFileMetaData::Property::Title, query.value(u"title"_s).toString());
    properties.insert(KFileMetaData::Property::Duration, query.value(u"duration"_s).toInt());
    properties.insert(KFileMetaData::Property::Width, query.value(u"width"_s).toInt());
    properties.insert(KFileMetaData::Property::Height, query.value(u"height"_s).toInt());
    properties.insert(KFileMetaData::Property::AspectRatio, query.value(u"aspect_ratio"_s).toDouble());
    properties.insert(KFileMetaData::Property::FrameRate, query.value(u"framerate"_s).toDouble());
    properties.insert(KFileMetaData::Property::VideoCodec, query.value(u"video_codec"_s).toString());
    properties.insert(KFileMetaData::Property::ImageOrientation, query.value(u"orientation"_s).toInt());
    properties.insert(KFileMetaData::Property::TrackNumber, query.value(u"track_no"_s).toInt());
    properties.insert(KFileMetaData::Property::DiscNumber, query.value(u"disc_no"_s).toInt());
    properties.insert(KFileMetaData::Property::Album, query.value(u"album"_s).toString());
    properties.insert(KFileMetaData::Property::Artist, query.value(u"artist"_s).toString());
    properties.insert(KFileMetaData::Property::AlbumArtist, query.value(u"album_artist"_s).toString());
    properties.insert(KFileMetaData::Property::AudioCodec, query.value(u"audio_codec"_s).toString());
    properties.insert(KFileMetaData::Property::BitRate, query.value(u"bitrate"_s).toInt());
    properties.insert(KFileMetaData::Property::SampleRate, query.value(u"sample_rate"_s).toInt());
    properties.insert(KFileMetaData::Property::Genre, query.value(u"genre"_s).toString());
    properties.insert(KFileMetaData::Property::ReleaseYear, query.value(u"release_year"_s).toInt());
    properties.insert(KFileMetaData::Property::Composer, query.value(u"composer"_s).toString());
    properties.insert(KFileMetaData::Property::Lyricist, query.value(u"lyricist"_s).toString());

    CachedMetadata cachedMetadata;
    cachedMetadata.properties = properties;
    cachedMetadata.url = url;
    cachedMetadata.metadataId = query.value(u"metadata_id").toInt();

    return cachedMetadata;
}

bool Database::deleteMetadata(const QUrl &url)
{
    QSqlQuery query(connection());
    query.prepare(u"DELETE FROM " % METADATA_CACHE_TABLE % u" WHERE url = :url");
    query.bindValue(u":url"_s, url);

    const auto result = query.exec();
    if (!result) {
        qDebug() << "Database::getMetadata:" << query.lastError().text() << getLastExecutedQuery(query);
    }

    return result;
}

bool Database::deleteAllMetadata()
{
    QSqlQuery query(connection());
    query.prepare(u"DELETE FROM " % METADATA_CACHE_TABLE);

    const auto result = query.exec();
    if (!result) {
        qDebug() << "Database::getMetadata:" << query.lastError().text() << getLastExecutedQuery(query);
    }

    return result;
}

#include "moc_database.cpp"
