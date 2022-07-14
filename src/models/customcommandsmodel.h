/*
 * SPDX-FileCopyrightText: 2021 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CUSTOMCOMMANDSMODEL_H
#define CUSTOMCOMMANDSMODEL_H

#include <QAbstractListModel>
#include <QSortFilterProxyModel>
#include <KSharedConfig>

class ProxyCustomCommandsModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit ProxyCustomCommandsModel(QObject *parent = nullptr);
};

class CustomCommandsModel : public QAbstractListModel
{
    Q_OBJECT

    struct Command {
        QString commandId;
        QString command;
        QString osdMessage;
        QString type;
        bool loadOnStartup {true};
        int order {-1};
    };

public:
    explicit CustomCommandsModel(QObject *parent = nullptr);

    enum Roles {
        CommandIdRole = Qt::UserRole + 1,
        CommandRole,
        OsdMessageRole,
        TypeRole,
        LoadOnStartupRole,
    };

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void moveRows(int oldIndex, int newIndex);
    Q_INVOKABLE void saveCustomCommand(const QString &command,
                                       const QString &osdMessage,
                                       const QString &type);
    Q_INVOKABLE void editCustomCommand(int row,
                                       const QString &command,
                                       const QString &osdMessage,
                                       const QString &type);
    Q_INVOKABLE void toggleCustomCommand(const QString &groupName, int row, bool loadOnStartup);
    Q_INVOKABLE void deleteCustomCommand(const QString &groupName, int row);

private:
    KSharedConfig::Ptr m_customCommandsConfig;
    QList<Command *> m_customCommands;
};

#endif // CUSTOMCOMMANDSMODEL_H
