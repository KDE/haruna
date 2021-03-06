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

class KRecentFilesAction;

struct RecentFile {
    QUrl url;
    QString name;
};

class RecentFilesModel : public QAbstractListModel
{
    Q_OBJECT
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
    Q_INVOKABLE void populate();

private:
    KRecentFilesAction *m_recentFilesAction;
    QList<RecentFile> m_urls;
    KConfigGroup m_recentFilesConfigGroup;
};

#endif // RECENTFILESMODEL_H
