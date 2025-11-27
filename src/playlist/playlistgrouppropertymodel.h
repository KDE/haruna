/*
 * SPDX-FileCopyrightText: 2025 Muhammet Sadık Uğursoy <sadikugursoy@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef PLAYLISTGROUPPROPERTYMODEL_H
#define PLAYLISTGROUPPROPERTYMODEL_H

#include <QAbstractListModel>
#include <QtQml/qqmlregistration.h>

struct GroupProperty {
    enum Type {
        StringType,
        IntType,
        RealType,
    };

    QString text;
    int group{0};
    int category{0};
    int display{0};
    int order{0};
    Type type{Type::StringType};
};

class PlaylistGroupPropertyModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit PlaylistGroupPropertyModel(QObject *parent = nullptr);
    friend class PlaylistSortProxyModel;

    enum Roles {
        LabelRole = Qt::UserRole,
        GroupRole,
        CategoryRole,
        LabelDisplayRole,
        OrderRole,
    };
    Q_ENUM(Roles)

    enum GroupSortOrder {
        SameAsPrimary,
        Ascending,
        Descending,
    };
    Q_ENUM(GroupSortOrder)

    enum GroupDisplay {
        AlwaysDisplay,
        PartialDisplay,
        NoDisplay,
    };
    Q_ENUM(GroupDisplay)

    enum GroupCategory {
        All = Qt::UserRole,
        FileCategory,
        AudioCategory,
        VideoCategory,
        SeparatorCategory,
    };
    Q_ENUM(GroupCategory)

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::UserRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild) override;

    GroupProperty takeGroupProperty(const int group);
    void appendGroupProperty(GroupProperty property);
    void removeProperty(uint index);

    Q_INVOKABLE bool hasProperty(const int group);
    Q_INVOKABLE void moveGroupProperty(int sourceRow, int destinationRow);

Q_SIGNALS:
    void propertiesChanged();

private:
    QList<GroupProperty> m_properties;
};
#endif // PLAYLISTGROUPPROPERTYMODEL_H
