/*
 * SPDX-FileCopyrightText: 2026 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef FONTSMODEL_H
#define FONTSMODEL_H

#include <QStringListModel>
#include <qqmlintegration.h>

class FontsModel : public QStringListModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit FontsModel(QObject *parent = nullptr);
    enum class Roles {
        FontNameRole = Qt::DisplayRole,
    };
    Q_ENUM(Roles)

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    Q_INVOKABLE void setStringList(const QStringList &strings);
};

#endif // FONTSMODEL_H
