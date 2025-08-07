/*
 * SPDX-FileCopyrightText: 2021 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef RECENTFILESMODEL_H
#define RECENTFILESMODEL_H

#include <QAbstractListModel>
#include <QUrl>
#include <QtQml/qqmlregistration.h>

#include "recentfile.h"

class RecentFilesModel : public QAbstractListModel
{
    Q_OBJECT
    QML_NAMED_ELEMENT(RecentFilesModel)

public:
    explicit RecentFilesModel(QObject *parent = nullptr);

    enum Roles {
        UrlRole = Qt::UserRole + 1,
        NameRole,
        OpenedFromRole,
    };

    enum class OpenedFrom {
        Other,
        OpenAction,
        ExternalApp,
        Playlist,
    };
    Q_ENUM(OpenedFrom)

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    Q_INVOKABLE void addUrl(const QUrl &url, OpenedFrom openedFrom, const QString &name = QString());
    Q_INVOKABLE void clear();
    Q_INVOKABLE void deleteEntries();
    Q_INVOKABLE void getItems();

private:
    void getHttpItemInfo(const QUrl &url, OpenedFrom openedFrom);
    QString openedFromToString(OpenedFrom from) const;
    OpenedFrom stringToOpenedFrom(const QString &from) const;
    QList<RecentFile> m_data;
};

#endif // RECENTFILESMODEL_H
