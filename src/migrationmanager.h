/*
 * SPDX-FileCopyrightText: 2025 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef MIGRATIONMANAGER_H
#define MIGRATIONMANAGER_H

#include <QObject>

class QSqlDatabase;

class MigrationManager : public QObject
{
    Q_OBJECT
public:
    explicit MigrationManager(QObject *parent = nullptr);

    void migrate(QSqlDatabase &db);

private:
    void migrateToV2(QSqlDatabase &db);
    void runMigrations(QSqlDatabase &db, const QStringList &migrations, int version);
    int databaseVersion(QSqlDatabase &db) const;
    bool setDatabaseVersion(QSqlDatabase &db, int version);
};

#endif // MIGRATIONMANAGER_H
