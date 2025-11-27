/*
 * SPDX-FileCopyrightText: 2025 Muhammet Sadık Uğursoy <sadikugursoy@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "playlistgrouppropertyproxymodel.h"
#include "playlistgrouppropertymodel.h"

PlaylistGroupPropertyProxyModel::PlaylistGroupPropertyProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    // Always auto-sort in ascending order by label and ignore case
    setDynamicSortFilter(true);
    setSortRole(PlaylistGroupPropertyModel::LabelRole);
    setSortCaseSensitivity(Qt::CaseInsensitive);
    setFilterCaseSensitivity(Qt::CaseInsensitive);
    sort(0, Qt::AscendingOrder);
}

QString PlaylistGroupPropertyProxyModel::searchText()
{
    return filterRegularExpression().pattern();
}

void PlaylistGroupPropertyProxyModel::setSearchText(QString text)
{
    setFilterRole(PlaylistGroupPropertyModel::LabelRole);
    setFilterRegularExpression(text);
    invalidateFilter();
    Q_EMIT searchTextChanged();
}

uint PlaylistGroupPropertyProxyModel::filterCategory()
{
    return m_filterCategory;
}

void PlaylistGroupPropertyProxyModel::setFilterCategory(uint category)
{
    m_filterCategory = category;
    invalidateFilter();
    Q_EMIT filterCategoryChanged();
}

bool PlaylistGroupPropertyProxyModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    // Sort by Category first, then by Label
    int leftCategory = sourceModel()->data(source_left, PlaylistGroupPropertyModel::CategoryRole).toInt();
    int rightCategory = sourceModel()->data(source_right, PlaylistGroupPropertyModel::CategoryRole).toInt();

    if (leftCategory == rightCategory) {
        return QSortFilterProxyModel::lessThan(source_left, source_right);
    }
    // Order: File > Audio > Video
    return leftCategory < rightCategory;
}

bool PlaylistGroupPropertyProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    // On top of search text, check the category
    uint category = sourceModel()->data(sourceModel()->index(source_row, 0), PlaylistGroupPropertyModel::CategoryRole).toUInt();

    bool searchMatch = QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
    bool categoryMatch = (m_filterCategory == Qt::DisplayRole) || (m_filterCategory == category);

    return searchMatch && categoryMatch;
}
#include "moc_playlistgrouppropertyproxymodel.cpp"
