/*
 * SPDX-FileCopyrightText: 2025 Muhammet Sadık Uğursoy <sadikugursoy@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "playlistsortpropertymodel.h"

PlaylistSortPropertyModel::PlaylistSortPropertyModel(QObject *parent)
    : QAbstractListModel{parent}
{
}

int PlaylistSortPropertyModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_properties.size();
}

QVariant PlaylistSortPropertyModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    auto property = m_properties.at(index.row());
    switch (role) {
    case LabelRole:
        return QVariant(property.text);
    case SortRole:
        return QVariant(property.sort);
    case CategoryRole:
        return QVariant(property.category);
    case OrderRole:
        return QVariant(property.order);
    case HideBlankRole:
        return QVariant(property.hideBlank);
    }

    return QVariant();
}

QHash<int, QByteArray> PlaylistSortPropertyModel::roleNames() const
{
    // clang-format off
    QHash<int, QByteArray> roles = {
    {LabelRole,     QByteArrayLiteral("label")},
    {SortRole,      QByteArrayLiteral("sort")},
    {CategoryRole,  QByteArrayLiteral("category")},
    {OrderRole,     QByteArrayLiteral("order")},
    {HideBlankRole, QByteArrayLiteral("hideBlank")},
    };
    // clang-format on

    return roles;
}

bool PlaylistSortPropertyModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild)
{
    Q_UNUSED(sourceParent)
    Q_UNUSED(count)
    Q_UNUSED(destinationParent)

    if (sourceRow < 0 || sourceRow >= rowCount() || sourceRow == destinationChild) {
        return false;
    }
    if (destinationChild == -1) {
        destinationChild = 0;
    }
    if (destinationChild >= rowCount()) {
        destinationChild = rowCount() - 1;
    }

    if (sourceRow < destinationChild) {
        // TopDown
        if (beginMoveRows(sourceParent, sourceRow, sourceRow, destinationParent, destinationChild + 1)) {
            m_properties.move(sourceRow, destinationChild);
        }
        endMoveRows();
    } else {
        // BottomUp
        if (beginMoveRows(sourceParent, sourceRow, sourceRow, destinationParent, destinationChild)) {
            m_properties.move(sourceRow, destinationChild);
        }
        endMoveRows();
    }
    return true;
}

SortProperty PlaylistSortPropertyModel::takeSortProperty(const int sort)
{
    for (qsizetype i = 0; i < m_properties.size(); ++i) {
        SortProperty property = m_properties[i];
        if (property.sort == sort) {
            removeProperty(i);
            return property;
        }
    }
    return SortProperty();
}

void PlaylistSortPropertyModel::appendSortProperty(SortProperty property)
{
    beginInsertRows(QModelIndex(), m_properties.size(), m_properties.size());
    m_properties.push_back(property);
    endInsertRows();
    Q_EMIT propertiesChanged();
}

void PlaylistSortPropertyModel::removeProperty(uint index)
{
    beginRemoveRows(QModelIndex(), index, index);
    m_properties.erase(m_properties.begin() + index);
    endRemoveRows();
    Q_EMIT propertiesChanged();
}

bool PlaylistSortPropertyModel::hasProperty(const int sort)
{
    for (qsizetype i = 0; i < m_properties.size(); ++i) {
        SortProperty property = m_properties[i];
        if (property.sort == sort) {
            return true;
        }
    }
    return false;
}

void PlaylistSortPropertyModel::moveSortProperty(int sourceRow, int destinationRow)
{
    if (moveRows(QModelIndex(), sourceRow, 1, QModelIndex(), destinationRow)) {
        Q_EMIT propertiesChanged();
    }
}

#include "moc_playlistsortpropertymodel.cpp"
