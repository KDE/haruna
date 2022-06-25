/*
 * SPDX-FileCopyrightText: 2021 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef RECENTFILESMODEL_H
#define RECENTFILESMODEL_H

#include <KConfigGroup>
#include <QAbstractListModel>

class KRecentFilesAction;
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
    Q_INVOKABLE void addUrl(const QString &path);
    Q_INVOKABLE void clear();

private:
    KRecentFilesAction *m_recentFilesAction;
    QList<QUrl> m_urls;
    KConfigGroup m_recentFilesConfigGroup;
};

#endif // RECENTFILESMODEL_H
