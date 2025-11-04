/*
 * SPDX-FileCopyrightText: 2025 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef PLAYLISTSORTPROXYMODEL_H
#define PLAYLISTSORTPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QtQml/qqmlregistration.h>

class PlaylistSortProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
    QML_NAMED_ELEMENT(PlaylistSortProxyModel)

public:
    explicit PlaylistSortProxyModel(QObject *parent = nullptr);

    enum class Sort {
        NameAscending,
        NameDescending,
        DurationAscending,
        DurationDescending,
    };
    Q_ENUM(Sort)

    void sortItems(Sort sortMode);
};

#endif // PLAYLISTSORTPROXYMODEL_H
