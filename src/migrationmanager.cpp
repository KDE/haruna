/*
 * SPDX-FileCopyrightText: 2025 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "migrationmanager.h"

#include <QFile>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

#include "logging/database.h"

using namespace Qt::StringLiterals;

MigrationManager::MigrationManager(QObject *parent)
    : QObject{parent}
{
}

void MigrationManager::migrate(QSqlDatabase &db)
{
    migrateToV2(db);
}

void MigrationManager::migrateToV2(QSqlDatabase &db)
{
    int newVersion = 2;
    if (databaseVersion(db) >= newVersion) {
        return;
    }

    QStringList migrations;
    QString migration;

    {
        QFile sqlFile(u":sql/v2/create-urls-table.sql"_s);
        if (!sqlFile.open(QFile::ReadOnly)) {
            qCDebug(HarunaDatabase) << sqlFile.fileName() << sqlFile.errorString();
        }
        migration = QString::fromUtf8(sqlFile.readAll());
        migrations.append(migration);
        sqlFile.close();
    }

    migration = u"DROP TABLE metadata_cache"_s;
    migrations.append(migration);

    {
        QFile sqlFile(u":sql/v2/create-metadata_cache-table.sql"_s);
        if (!sqlFile.open(QFile::ReadOnly)) {
            qCDebug(HarunaDatabase) << sqlFile.fileName() << sqlFile.errorString();
        }
        migration = QString::fromUtf8(sqlFile.readAll());
        migrations.append(migration);
        sqlFile.close();
    }

    migration = u"DROP TABLE recent_files"_s;
    migrations.append(migration);

    {
        QFile sqlFile(u":sql/v2/create-recent_files-table.sql"_s);
        if (!sqlFile.open(QFile::ReadOnly)) {
            qCDebug(HarunaDatabase) << sqlFile.fileName() << sqlFile.errorString();
        }
        migration = QString::fromUtf8(sqlFile.readAll());
        migrations.append(migration);
        sqlFile.close();
    }

    runMigrations(db, migrations, newVersion);
}

void MigrationManager::runMigrations(QSqlDatabase &db, const QStringList &migrations, int version)
{
    if (!db.transaction()) {
        qWarning() << "Failed to start transaction";
        return;
    }

    for (const auto &migration : migrations) {
        QSqlQuery query(db);
        if (!query.exec(migration)) {
            db.rollback();
            qWarning() << "Migration failed for version" << version << query.lastError() << query.lastQuery();
            return;
        }
    }

    if (!setDatabaseVersion(db, version)) {
        db.rollback();
        qWarning() << "Failed to set DB version";
        return;
    }

    db.commit();
}

int MigrationManager::databaseVersion(QSqlDatabase &db) const
{
    QSqlQuery query(db);
    if (!query.exec(u"PRAGMA user_version;"_s) || !query.next()) {
        qWarning() << "Failed to read DB version:" << query.lastError();
        return 0;
    }
    return query.value(0).toInt();
}

bool MigrationManager::setDatabaseVersion(QSqlDatabase &db, int version)
{
    QSqlQuery query(db);
    if (!query.exec(u"PRAGMA user_version = %1;"_s.arg(version))) {
        qCDebug(HarunaDatabase) << query.lastError() << query.lastQuery();
        return false;
    }
    return true;
}

#include "moc_migrationmanager.cpp"