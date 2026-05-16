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
        return {};
    }

    const auto &property = m_properties.at(index.row());
    switch (role) {
    case LabelRole:
        return property.text;
    case SortRole:
        return property.sort;
    case CategoryRole:
        return property.category;
    case OrderRole:
        return property.order;
    case HideBlankRole:
        return property.hideBlank;
    }

    return {};
}

QHash<int, QByteArray> PlaylistSortPropertyModel::roleNames() const
{
    // clang-format off
    static QHash<int, QByteArray> roles = {
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
        SortProperty property = m_properties.at(i);
        if (property.sort == sort) {
            removeProperty(i);
            return property;
        }
    }
    return {};
}

void PlaylistSortPropertyModel::appendSortProperty(const SortProperty &property)
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
    return std::any_of(m_properties.constBegin(), m_properties.constEnd(), [sort](const SortProperty &sortProperty) {
        return sortProperty.sort == sort;
    });
}

void PlaylistSortPropertyModel::moveSortProperty(int sourceRow, int destinationRow)
{
    if (moveRows(QModelIndex(), sourceRow, 1, QModelIndex(), destinationRow)) {
        Q_EMIT propertiesChanged();
    }
}

#include "moc_playlistsortpropertymodel.cpp"
