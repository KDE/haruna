#include "database.h"

#include <QFile>
#include <QQmlEngine>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStringBuilder>

#include "global.h"
#include "recentfile.h"

using namespace Qt::StringLiterals;

const QString RECENT_FILES_TABLE = u"recent_files"_s;

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
    const auto dbFile{Global::instance()->appConfigFilePath(Global::ConfigFile::Database)};
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
        qDebug() << 11;
        QFile sqlFile(u":sql/create-recent_files-table.sql"_s);
        if (sqlFile.open(QFile::ReadOnly)) {
            qDebug() << 22;
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
    query.exec(u"DELETE FROM recent_files"_s);
}

#include "moc_database.cpp"
