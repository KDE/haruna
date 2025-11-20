/*
 * SPDX-FileCopyrightText: 2025 George Florea Bănuș <georgefb899@gmail.com>
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

bool PlaylistSortProxyModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    int primarySortRole = sortRole();
    QVariant left = sourceModel()->data(source_left, primarySortRole);
    QVariant right = sourceModel()->data(source_right, primarySortRole);
    int compare;

    if (primarySortRole == Sort::FileSize) {
        int l = left.toInt();
        int r = right.toInt();
        compare = l - r;
    } else {
        compare = QString::compare(left.toString(), right.toString());
    }

    switch (m_sortingOrder) {
    case Qt::AscendingOrder:
        return compare < 0;
    case Qt::DescendingOrder:
        return compare > 0;
    }
    return source_left.row() < source_right.row(); // fallback
}

void PlaylistSortProxyModel::sortItems(Sort role)
{
    invalidate();
    m_sortRole = role;
    if (role == Sort::None) {
        // Revert any sorting done and reflect playlistmodel
        sort(-1, Qt::SortOrder::AscendingOrder);
        return;
    }
    setSortRole(m_sortRole);
    // Note that Qt::SortOrder::AscendingOrder is given here regardless of m_sortingOrder
    // because  the order will be handled inside custom lessThan() method.
    sort(0, Qt::SortOrder::AscendingOrder);
}

void PlaylistSortProxyModel::sortItems(Qt::SortOrder order)
{
    m_sortingOrder = order;
    sortItems(m_sortRole);
}

#include "moc_playlistsortproxymodel.cpp"
