/*
 * SPDX-FileCopyrightText: 2026 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "seekablerangesmodel.h"

using namespace Qt::StringLiterals;

SeekableRangesModel::SeekableRangesModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int SeekableRangesModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return m_data.size();
}

QVariant SeekableRangesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    const auto &item = m_data.at(index.row());

    switch (role) {
    case static_cast<int>(Roles::StartRole):
        return item.start;
    case static_cast<int>(Roles::EndRole):
        return item.end;
    }
    return QVariant();
}

QHash<int, QByteArray> SeekableRangesModel::roleNames() const
{
    // clang-format off
    static QHash<int, QByteArray> roles{
        {static_cast<int>(Roles::StartRole), QByteArrayLiteral("start")},
        {static_cast<int>(Roles::EndRole),   QByteArrayLiteral("end")},
    };
    // clang-format on

    return roles;
}

void SeekableRangesModel::populate(const QList<QVariant> data)
{
    const auto newSize = data.size();
    const auto oldSize = m_data.size();

    // sync m_data size with data size
    if (newSize < oldSize) {
        beginRemoveRows(QModelIndex(), newSize, oldSize - 1);
        m_data.resize(newSize);
        endRemoveRows();
    } else if (newSize > oldSize) {
        beginInsertRows(QModelIndex(), oldSize, newSize - 1);
        m_data.resize(newSize);
        endInsertRows();
    }

    // update values
    if (newSize > 0) {
        for (int i = 0; i < newSize; ++i) {
            QVariantMap map = data.at(i).toMap();
            m_data[i].start = map.value(u"start"_s).toDouble();
            m_data[i].end = map.value(u"end"_s).toDouble();
        }

        Q_EMIT dataChanged(index(0), index(newSize - 1));
    }
}

#include "moc_seekablerangesmodel.cpp"
