/*
 * SPDX-FileCopyrightText: 2025 Muhammet Sadık Uğursoy <sadikugursoy@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "playlistgrouppropertymodel.h"

PlaylistGroupPropertyModel::PlaylistGroupPropertyModel(QObject *parent)
    : QAbstractListModel{parent}
{
}

int PlaylistGroupPropertyModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_properties.size();
}

QVariant PlaylistGroupPropertyModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    auto property = m_properties.at(index.row());
    switch (role) {
    case LabelRole:
        return QVariant(property.text);
    case GroupRole:
        return QVariant(property.group);
    case CategoryRole:
        return QVariant(property.category);
    case LabelDisplayRole:
        return QVariant(property.display);
    case OrderRole:
        return QVariant(property.order);
    }

    return QVariant();
}

QHash<int, QByteArray> PlaylistGroupPropertyModel::roleNames() const
{
    // clang-format off
    QHash<int, QByteArray> roles = {
    {LabelRole,         QByteArrayLiteral("label")},
    {GroupRole,         QByteArrayLiteral("group")},
    {CategoryRole,      QByteArrayLiteral("category")},
    {LabelDisplayRole,  QByteArrayLiteral("display")},
    {OrderRole,         QByteArrayLiteral("order")},
    };
    // clang-format on

    return roles;
}

bool PlaylistGroupPropertyModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild)
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

GroupProperty PlaylistGroupPropertyModel::takeGroupProperty(const int group)
{
    for (qsizetype i = 0; i < m_properties.size(); ++i) {
        GroupProperty property = m_properties[i];
        if (property.group == group) {
            removeProperty(i);
            return property;
        }
    }
    return GroupProperty();
}

void PlaylistGroupPropertyModel::appendGroupProperty(GroupProperty property)
{
    beginInsertRows(QModelIndex(), m_properties.size(), m_properties.size());
    m_properties.push_back(property);
    endInsertRows();
    Q_EMIT propertiesChanged();
}

void PlaylistGroupPropertyModel::removeProperty(uint index)
{
    beginRemoveRows(QModelIndex(), index, index);
    m_properties.erase(m_properties.begin() + index);
    endRemoveRows();
    Q_EMIT propertiesChanged();
}

bool PlaylistGroupPropertyModel::hasProperty(const int group)
{
    for (qsizetype i = 0; i < m_properties.size(); ++i) {
        GroupProperty property = m_properties[i];
        if (property.group == group) {
            return true;
        }
    }
    return false;
}

void PlaylistGroupPropertyModel::moveGroupProperty(int sourceRow, int destinationRow)
{
    if (moveRows(QModelIndex(), sourceRow, 1, QModelIndex(), destinationRow)) {
        Q_EMIT propertiesChanged();
    }
}

#include "moc_playlistgrouppropertymodel.cpp"
