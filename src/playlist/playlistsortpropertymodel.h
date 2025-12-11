/*
 * SPDX-FileCopyrightText: 2025 Muhammet Sadık Uğursoy <sadikugursoy@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef PLAYLISTSORTPROPERTYMODEL_H
#define PLAYLISTSORTPROPERTYMODEL_H

#include <QAbstractListModel>
#include <QtQml/qqmlregistration.h>

struct SortProperty {
    enum Type {
        StringType,
        IntType,
        RealType,
        DateType,
    };

    QString text;
    int sort{0};
    int category{0};
    int order{0};
    bool hideBlank{false};
    Type type{Type::StringType};
};

class PlaylistSortPropertyModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit PlaylistSortPropertyModel(QObject *parent = nullptr);
    friend class PlaylistSortProxyModel;

    enum Roles {
        LabelRole = Qt::UserRole,
        SortRole,
        CategoryRole,
        OrderRole,
        HideBlankRole,
    };
    Q_ENUM(Roles)

    enum SortOrder {
        SameAsPrimary,
        Ascending,
        Descending,
    };
    Q_ENUM(SortOrder)

    enum Category {
        All = Qt::UserRole,
        FileCategory,
        AudioCategory,
        VideoCategory,
        SeparatorCategory,
    };
    Q_ENUM(Category)

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::UserRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild) override;

    SortProperty takeSortProperty(const int group);
    void appendSortProperty(SortProperty property);
    void removeProperty(uint index);

    Q_INVOKABLE bool hasProperty(const int group);
    Q_INVOKABLE void moveSortProperty(int sourceRow, int destinationRow);

Q_SIGNALS:
    void propertiesChanged();

private:
    QList<SortProperty> m_properties;
};
#endif // PLAYLISTSORTPROPERTYMODEL_H
