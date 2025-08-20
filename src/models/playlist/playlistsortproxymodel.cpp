/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "playlistsortproxymodel.h"
#include "playlistmodel.h"

PlaylistSortProxyModel::PlaylistSortProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setDynamicSortFilter(true);
}

void PlaylistSortProxyModel::sortItems(Sort sortMode)
{
    switch (sortMode) {
    case Sort::NameAscending: {
        setSortRole(PlaylistModel::NameRole);
        sort(0, Qt::AscendingOrder);
        break;
    }
    case Sort::NameDescending: {
        setSortRole(PlaylistModel::NameRole);
        sort(0, Qt::DescendingOrder);
        break;
    }
    case Sort::DurationAscending: {
        setSortRole(PlaylistModel::DurationRole);
        sort(0, Qt::AscendingOrder);
        break;
    }
    case Sort::DurationDescending: {
        setSortRole(PlaylistModel::DurationRole);
        sort(0, Qt::DescendingOrder);
        break;
    }
    }
}

#include "moc_playlistsortproxymodel.cpp"
