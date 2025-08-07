#ifndef RECENTFILE_H
#define RECENTFILE_H

#include <QString>
#include <QUrl>

struct RecentFile {
    uint id{0};
    QUrl url;
    QString filename;
    QString openedFrom;
    qint64 timestamp{0};
};

#endif // RECENTFILE_H
