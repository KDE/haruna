/*
 * SPDX-FileCopyrightText: 2025 Muhammet Sadık Uğursoy <sadikugursoy@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "playlistproxymodel.h"

struct Range {
    uint first;
    uint last;
    Range(uint a, uint b)
    {
        first = a;
        last = b;
    }
    uint length() const
    {
        return last - first + 1;
    };
    friend bool operator<(Range const &c1, Range const &c2)
    {
        return c1.first < c2.first;
    };
};

PlaylistProxyModel::PlaylistProxyModel(QObject *parent)
    : QAbstractProxyModel(parent)
{
}

void PlaylistProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    beginResetModel();

    connect(sourceModel, &QAbstractItemModel::dataChanged, this, &PlaylistProxyModel::onDataChanged);
    connect(sourceModel, &QAbstractItemModel::rowsAboutToBeInserted, this, &PlaylistProxyModel::onRowsAboutToBeInserted);
    connect(sourceModel, &QAbstractItemModel::rowsInserted, this, &PlaylistProxyModel::onRowsInserted);
    connect(sourceModel, &QAbstractItemModel::rowsAboutToBeRemoved, this, &PlaylistProxyModel::onRowsAboutToBeRemoved);
    connect(sourceModel, &QAbstractItemModel::rowsRemoved, this, &PlaylistProxyModel::onRowsRemoved);
    connect(sourceModel, &QAbstractItemModel::modelAboutToBeReset, this, &PlaylistProxyModel::onModelAboutToBeReset);
    connect(sourceModel, &QAbstractItemModel::modelReset, this, &PlaylistProxyModel::onModelReset);
    connect(sourceModel, &QAbstractItemModel::layoutAboutToBeChanged, this, &PlaylistProxyModel::onLayoutAboutToBeChanged);
    connect(sourceModel, &QAbstractItemModel::layoutChanged, this, &PlaylistProxyModel::onLayoutChanged);

    QAbstractProxyModel::setSourceModel(sourceModel);

    endResetModel();
}

QVariant PlaylistProxyModel::data(const QModelIndex &index, int role) const
{
    return sourceModel()->data(mapToSource(index), role);
}

QModelIndex PlaylistProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return QModelIndex();
    }
    return createIndex(row, column);
}

QModelIndex PlaylistProxyModel::parent(const QModelIndex & /* child */) const
{
    return QModelIndex();
}

int PlaylistProxyModel::rowCount(const QModelIndex & /* parent */) const
{
    return sourceModel()->rowCount();
}

// ListView does not care about this but QSortFilterProxyModel does. Must override to avoid SEGFAULTs.
int PlaylistProxyModel::columnCount(const QModelIndex & /* parent */) const
{
    return 1;
}

QModelIndex PlaylistProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    if (!proxyIndex.isValid()) {
        return QModelIndex();
    }
    return sourceModel()->index(remapRowToSource(proxyIndex.row()), 0);
}

QModelIndex PlaylistProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    if (!sourceIndex.isValid()) {
        return QModelIndex();
    }

    return index(remapRowFromSource(sourceIndex.row()), 0);
}

// Note that unlike the signature function, row and destinationRow are local indices. Calculated inside FilterProxyModel
// We always expect 1 row movement so count is ignored.
bool PlaylistProxyModel::moveRows(const QModelIndex &sourceParent, int row, int count, const QModelIndex &destinationParent, int destinationRow)
{
    Q_UNUSED(sourceParent)
    Q_UNUSED(count)
    Q_UNUSED(destinationParent)

    if (row < 0 || row >= rowCount()) {
        return false;
    }

    if (destinationRow == -1) {
        destinationRow = rowCount() - 1;
    }

    m_layout.move(row, destinationRow);
    return true;
}

void PlaylistProxyModel::setInsertOffset(uint offset)
{
    m_insertOffset = offset;
    m_insert = true;
}

void PlaylistProxyModel::resetLayout()
{
    std::sort(m_layout.begin(), m_layout.end());
    Q_EMIT dataChanged(index(0, 0), index(rowCount() - 1, 0));
}

int PlaylistProxyModel::remapRowToSource(int row) const
{
    if (row < m_layout.size()) {
        return m_layout[row];
    }
    return row;
}

int PlaylistProxyModel::remapRowFromSource(int row) const
{
    if (row < m_layout.size()) {
        return m_layout.indexOf(row);
    }
    return row;
}

void PlaylistProxyModel::onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    auto mappedTopLeft = mapFromSource(topLeft);
    auto mappedBottomRight = mapFromSource(bottomRight);

    Q_EMIT dataChanged(mappedTopLeft, mappedBottomRight, roles);
}

void PlaylistProxyModel::onRowsAboutToBeInserted(const QModelIndex &, int first, int last)
{
    // After QSortFilterProxyModel inserts the element, it sorts itself. We need to update the indices
    // that are bigger than the new row index to keep the models synced.
    // Iterate the list and increment all indices that are greater than the indices of the new rows
    uint rowsInserted = last - first + 1;
    for (auto itemRow = m_layout.begin(); itemRow != m_layout.end(); ++itemRow) {
        if (*itemRow >= uint(first)) {
            *itemRow += rowsInserted;
        }
    }

    int insertPosition;
    if (m_insert) {
        insertPosition = m_insertOffset;
    } else {
        insertPosition = m_layout.size();
    }

    int local_first = insertPosition;
    int local_last = insertPosition + last - first;

    beginInsertRows(QModelIndex(), local_first, local_last);
    int offset = 0; // we are adding items one-by-one for now, so this is unnecessary. But it doesn't hurt.
    for (int i = first; i <= last; ++i) {
        m_layout.insert(insertPosition + offset++, i);
    }
    endInsertRows();
    // Reset insert mode
    m_insert = false;
}

void PlaylistProxyModel::onRowsInserted(const QModelIndex &, int /*first*/, int /*last*/)
{
}

void PlaylistProxyModel::onRowsAboutToBeRemoved(const QModelIndex &, int first, int last)
{
    auto rowRangeMaker = [](QList<uint> &rows) {
        QList<Range> rowRanges;
        uint _last = rows.back();
        uint _first = _last;
        rows.pop_back();
        while (!rows.empty()) {
            uint _temp = rows.back();
            rows.pop_back();
            if (_temp + 1 == _first) {
                _first = _temp;
                continue;
            }
            rowRanges.append(Range(_first, _last));
            _last = _temp;
            _first = _last;
        }
        rowRanges.append(Range(_first, _last));
        return rowRanges;
    };

    // first and last parameters corresponds to SortFilterProxy rows. Deleted row range in there
    // does not necessarily make a continuous range in here. We need to check it.
    QList<uint> localRows;
    for (int i = first; i < last + 1; i++) {
        localRows.append(remapRowFromSource(i));
    }
    std::sort(localRows.begin(), localRows.end());

    QList<Range> rowRanges = rowRangeMaker(localRows);
    // Before moving the rows, we need to detect how the deleted indices affect other indices
    // To do that, we check deleted source rows (SFPM rows) and we will update every mapping we have
    // that are greater than the removed rows.
    // For example, if the source row (2, 4) is deleted, all the following indices in the SFPM
    // will be decreased by 3. (Indices 5,6,7... will become 2,3,4...) So our mappings to the old indices
    // must be updated accordingly by reducing the same amount from those indices
    uint deltaRow = last - first + 1;
    for (auto itemRow = m_layout.begin(); itemRow != m_layout.end(); ++itemRow) {
        if (*itemRow > uint(last)) {
            *itemRow -= deltaRow;
        }
    }

    for (auto range : std::as_const(rowRanges)) {
        beginRemoveRows(QModelIndex(), range.first, range.last);
        m_layout.erase(m_layout.begin() + range.first, m_layout.begin() + range.last + 1);
        endRemoveRows();
    }
}

void PlaylistProxyModel::onRowsRemoved(const QModelIndex &, int /*first*/, int /*last*/)
{
}

void PlaylistProxyModel::onModelAboutToBeReset()
{
    beginResetModel();
    m_layout.clear();
    endResetModel();
    Q_EMIT layoutAboutToBeChanged();
}

void PlaylistProxyModel::onModelReset()
{
    Q_EMIT layoutChanged();
}

void PlaylistProxyModel::onLayoutAboutToBeChanged()
{
    // When this signal is received, either of the model layers are changed
    // meaning we need to reset out mappings
    beginResetModel();
    resetLayout();
    endResetModel();
    Q_EMIT layoutAboutToBeChanged();
}

void PlaylistProxyModel::onLayoutChanged()
{
    Q_EMIT layoutChanged();
}

#include "moc_playlistproxymodel.cpp"
