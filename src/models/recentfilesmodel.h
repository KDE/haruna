/*
 * SPDX-FileCopyrightText: 2021 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef RECENTFILESMODEL_H
#define RECENTFILESMODEL_H

#include <KConfigGroup>

#include <QAbstractListModel>
#include <QUrl>
#include <QtQml/qqmlregistration.h>

class KRecentFilesAction;

struct RecentFile {
    QUrl url;
    QString name;
};

class RecentFilesModel : public QAbstractListModel
{
    Q_OBJECT
    QML_NAMED_ELEMENT(RecentFilesModel)

public:
    explicit RecentFilesModel(QObject *parent = nullptr);

    enum Roles {
        PathRole = Qt::UserRole + 1,
        NameRole,
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    Q_INVOKABLE void addUrl(const QString &path, const QString &name = QString());
    Q_INVOKABLE void clear();
    Q_INVOKABLE void deleteEntries();
    Q_INVOKABLE void populate();

    int maxRecentFiles() const;
    void setMaxRecentFiles(int _maxRecentFiles);

private:
    void saveEntries();
    void getHttpItemInfo(const QUrl &url);
    QList<RecentFile> m_urls;
    KConfigGroup m_recentFilesConfigGroup;
    int m_maxRecentFiles{10};
};

#endif // RECENTFILESMODEL_H
