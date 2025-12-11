/*
 * SPDX-FileCopyrightText: 2025 Muhammet Sadık Uğursoy <sadikugursoy@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef PLAYLISTSORTPROPERTYPROXYMODEL_H
#define PLAYLISTSORTPROPERTYPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QtQml/qqmlregistration.h>

class PlaylistSortPropertyProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit PlaylistSortPropertyProxyModel(QObject *parent = nullptr);

    Q_PROPERTY(QString searchText READ searchText WRITE setSearchText NOTIFY searchTextChanged)
    QString searchText();
    void setSearchText(QString text);

    Q_PROPERTY(uint filterCategory READ filterCategory WRITE setFilterCategory NOTIFY filterCategoryChanged)
    uint filterCategory();
    void setFilterCategory(uint category);

    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

Q_SIGNALS:
    void searchTextChanged();
    void filterCategoryChanged();

private:
    uint m_filterCategory{Qt::DisplayRole};
};

#endif // PLAYLISTSORTPROPERTYPROXYMODEL_H
