#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QtQml/qqmlregistration.h>

class QQmlEngine;
class QJSEngine;
struct QSqlDatabase;
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

private:
    Database(QObject *parent = nullptr);
    void createTables();

    Q_DISABLE_COPY_MOVE(Database)
};

#endif // DATABASE_H
