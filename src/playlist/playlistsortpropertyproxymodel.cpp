/*
 * SPDX-FileCopyrightText: 2025 Muhammet Sadık Uğursoy <sadikugursoy@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "playlistsortpropertyproxymodel.h"
#include "playlistsortpropertymodel.h"

PlaylistSortPropertyProxyModel::PlaylistSortPropertyProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    // Always auto-sort in ascending order by label and ignore case
    setDynamicSortFilter(true);
    setSortRole(PlaylistSortPropertyModel::LabelRole);
    setSortCaseSensitivity(Qt::CaseInsensitive);
    setFilterCaseSensitivity(Qt::CaseInsensitive);
    sort(0, Qt::AscendingOrder);
    setFilterCategory(PlaylistSortPropertyModel::Category::All);
}

QString PlaylistSortPropertyProxyModel::searchText()
{
    return filterRegularExpression().pattern();
}

void PlaylistSortPropertyProxyModel::setSearchText(QString text)
{
    setFilterRole(PlaylistSortPropertyModel::LabelRole);
    setFilterRegularExpression(text);
    invalidateFilter();
    Q_EMIT searchTextChanged();
}

uint PlaylistSortPropertyProxyModel::filterCategory()
{
    return m_filterCategory;
}

void PlaylistSortPropertyProxyModel::setFilterCategory(uint category)
{
    m_filterCategory = category;
    invalidateFilter();
    Q_EMIT filterCategoryChanged();
}

bool PlaylistSortPropertyProxyModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    // Sort by Category first, then by Label
    int leftCategory = sourceModel()->data(source_left, PlaylistSortPropertyModel::CategoryRole).toInt();
    int rightCategory = sourceModel()->data(source_right, PlaylistSortPropertyModel::CategoryRole).toInt();

    if (leftCategory == rightCategory) {
        return QSortFilterProxyModel::lessThan(source_left, source_right);
    }
    // Order: File > Audio > Video
    return leftCategory < rightCategory;
}

bool PlaylistSortPropertyProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    // On top of search text, check the category
    uint category = sourceModel()->data(sourceModel()->index(source_row, 0), PlaylistSortPropertyModel::CategoryRole).toUInt();

    bool searchMatch = QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
    bool categoryMatch = (m_filterCategory == PlaylistSortPropertyModel::Category::All) || (m_filterCategory == category);

    return searchMatch && categoryMatch;
}
#include "moc_playlistsortpropertyproxymodel.cpp"
