/*
 * SPDX-FileCopyrightText: 2025 Muhammet Sadık Uğursoy <sadikugursoy@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef PLAYLISTPROXYMODEL_H
#define PLAYLISTPROXYMODEL_H

#include <QAbstractProxyModel>
#include <QtQml/qqmlregistration.h>

class PlaylistProxyModel : public QAbstractProxyModel
{
    Q_OBJECT
    QML_NAMED_ELEMENT(PlaylistProxyModel)

public:
    explicit PlaylistProxyModel(QObject *parent = nullptr);
    friend class PlaylistFilterProxyModel;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    using QAbstractProxyModel::QAbstractProxyModel;
    void setSourceModel(QAbstractItemModel *sourceModel) override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;
    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;
    bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild) override;
    void setInsertOffset(uint offset);

private:
    // Callbacks
    void onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);
    void onRowsAboutToBeInserted(const QModelIndex &, int first, int last);
    void onRowsInserted(const QModelIndex &, int first, int last);
    void onRowsAboutToBeRemoved(const QModelIndex &, int first, int last);
    void onRowsRemoved(const QModelIndex &, int first, int last);
    void onModelAboutToBeReset();
    void onModelReset();
    void onLayoutAboutToBeChanged();
    void onLayoutChanged();

    void resetLayout();
    int remapRowToSource(int row) const;
    int remapRowFromSource(int row) const;

    QList<uint> m_layout;
    uint m_insertOffset{0};
    bool m_insert{false};
};

#endif // PLAYLISTPROXYMODEL_H
