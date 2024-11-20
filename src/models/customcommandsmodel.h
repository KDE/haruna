/*
 * SPDX-FileCopyrightText: 2021 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CUSTOMCOMMANDSMODEL_H
#define CUSTOMCOMMANDSMODEL_H

#include <QAbstractListModel>
#include <QKeySequence>
#include <QSortFilterProxyModel>
#include <QtQml/qqmlregistration.h>

#include <KSharedConfig>

class ActionsModel;

class CustomCommandsModel : public QAbstractListModel
{
    Q_OBJECT
    QML_NAMED_ELEMENT(CustomCommandsModel)

    Q_PROPERTY(ActionsModel *appActionsModel READ appActionsModel WRITE setAppActionsModel NOTIFY appActionsModelChanged)

    struct Command {
        QString commandId;
        QString command;
        QString osdMessage;
        QKeySequence shortcut;
        QString type;
        bool setOnStartup{true};
        int order{-1};
    };

public:
    explicit CustomCommandsModel(QObject *parent = nullptr);

    enum Roles {
        CommandIdRole = Qt::UserRole + 1,
        CommandRole,
        OsdMessageRole,
        TypeRole,
        ShortcutRole,
        SetOnStartupRole,
    };

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void init();
    Q_INVOKABLE bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild) override;
    Q_INVOKABLE void saveCustomCommand(const QString &command, const QString &osdMessage, const QString &type);
    Q_INVOKABLE void editCustomCommand(int row, const QString &command, const QString &osdMessage, const QString &type);
    Q_INVOKABLE void toggleCustomCommand(const QString &groupName, int row, bool setOnStartup);
    Q_INVOKABLE void deleteCustomCommand(const QString &groupName, int row);

    ActionsModel *appActionsModel();
    void setAppActionsModel(ActionsModel *_appActionsModel);

Q_SIGNALS:
    void appActionsModelChanged();

private:
    KSharedConfig::Ptr m_customCommandsConfig;
    QList<Command> m_customCommands;
    ActionsModel *m_appActionsModel{nullptr};
};

#endif // CUSTOMCOMMANDSMODEL_H
