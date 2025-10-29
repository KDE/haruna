/*
 * SPDX-FileCopyrightText: 2021 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "customcommandsmodel.h"

#include <KConfigGroup>

#include "actionsmodel.h"
#include "pathutils.h"

using namespace Qt::StringLiterals;

CustomCommandsModel::CustomCommandsModel(QObject *parent)
    : QAbstractListModel(parent)
{
    connect(this, &QAbstractListModel::rowsMoved, this, [=]() {
        for (int i = 0; i < m_customCommands.size(); ++i) {
            auto configGroup = m_customCommandsConfig->group(m_customCommands[i].commandId);
            configGroup.writeEntry("Order", i);
            configGroup.sync();
        }
    });
}

int CustomCommandsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_customCommands.size();
}

QVariant CustomCommandsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    Command command = m_customCommands[index.row()];

    switch (role) {
    case CommandIdRole:
        return QVariant(command.commandId);
    case CommandRole:
        return QVariant(command.command);
    case OsdMessageRole:
        return QVariant(command.osdMessage);
    case TypeRole:
        return QVariant(command.type);
    case ShortcutRole:
        return QVariant(command.shortcut);
    case SetOnStartupRole:
        return QVariant(command.setOnStartup);
    }

    return QVariant();
}

QHash<int, QByteArray> CustomCommandsModel::roleNames() const
{
    // clang-format off
    QHash<int, QByteArray> roles{
        {CommandIdRole,    QByteArrayLiteral("commandId")},
        {OsdMessageRole,   QByteArrayLiteral("osdMessage")},
        {CommandRole,      QByteArrayLiteral("command")},
        {TypeRole,         QByteArrayLiteral("type")},
        {ShortcutRole,     QByteArrayLiteral("shortcut")},
        {SetOnStartupRole, QByteArrayLiteral("setOnStartup")},
    };
    // clang-format on

    return roles;
}

void CustomCommandsModel::init()
{
    connect(appActionsModel(), &ActionsModel::shortcutChanged, this, [=](const QString &name, const QKeySequence &shortcut) {
        for (int i{0}; i < m_customCommands.count(); ++i) {
            if (m_customCommands[i].commandId == name) {
                m_customCommands[i].shortcut = shortcut;
                Q_EMIT dataChanged(index(i, 0), index(i, 0));
                return;
            }
        }
    });

    QString ccConfig = PathUtils::instance()->configFilePath(PathUtils::ConfigFile::CustomCommands);
    m_customCommandsConfig = KSharedConfig::openConfig(ccConfig, KConfig::SimpleConfig);
    QStringList groups = m_customCommandsConfig->groupList();

    beginInsertRows(QModelIndex(), 0, groups.size());
    for (const QString &groupName : std::as_const(groups)) {
        auto configGroup = m_customCommandsConfig->group(groupName);
        Command c;
        c.commandId = groupName;
        c.command = configGroup.readEntry("Command", QString());
        c.osdMessage = configGroup.readEntry("OsdMessage", QString());
        c.type = configGroup.readEntry("Type", QString());
        c.shortcut = appActionsModel()->getShortcut(c.commandId, QString());
        c.order = configGroup.readEntry("Order", 0);
        c.setOnStartup = configGroup.readEntry("SetOnStartup", true);
        m_customCommands << c;
        if (c.type == u"shortcut"_s) {
            Action action;
            action.name = c.commandId;
            action.text = c.command;
            action.description = c.osdMessage;
            action.shortcut = c.shortcut;
            action.type = u"CustomAction"_s;
            appActionsModel()->appendCustomAction(action);
        }
    }

    std::sort(m_customCommands.begin(), m_customCommands.end(), [=](Command c1, Command c2) {
        return c1.order < c2.order;
    });

    endInsertRows();
}

bool CustomCommandsModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild)
{
    if (sourceParent != destinationParent || sourceParent != QModelIndex()) {
        return false;
    }

    const bool isMoveDown = destinationChild > sourceRow;
    // QAbstractItemModel::beginMoveRows(): when moving rows down in the same parent,
    // the rows will be placed before the destinationChild index.
    auto destinationRow = isMoveDown ? destinationChild + 1 : destinationChild;
    if (!beginMoveRows(sourceParent, sourceRow, sourceRow + count - 1, destinationParent, destinationRow)) {
        return false;
    }

    for (int i = 0; i < count; i++) {
        m_customCommands.move(isMoveDown ? sourceRow : sourceRow + i, destinationChild);
    }

    endMoveRows();
    return true;
}

void CustomCommandsModel::saveCustomCommand(const QString &command, const QString &osdMessage, const QString &type)
{
    int counter = m_customCommandsConfig->group(QString()).readEntry("Counter", 0);
    const QString &groupName = QString(u"Command_%1"_s).arg(counter);

    if (m_customCommandsConfig->group(groupName).exists()) {
        return;
    }

    m_customCommandsConfig->group(groupName).writeEntry("Command", command);
    m_customCommandsConfig->group(groupName).writeEntry("OsdMessage", osdMessage);
    m_customCommandsConfig->group(groupName).writeEntry("Type", type);
    m_customCommandsConfig->group(groupName).writeEntry("Order", rowCount());
    m_customCommandsConfig->group(QString()).writeEntry("Counter", counter + 1);
    m_customCommandsConfig->sync();

    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    auto configGroup = m_customCommandsConfig->group(groupName);
    Command c;
    c.commandId = groupName;
    c.command = configGroup.readEntry("Command", QString());
    c.osdMessage = configGroup.readEntry("OsdMessage", QString()), c.type = configGroup.readEntry("Type", QString());
    m_customCommands << c;
    endInsertRows();

    if (c.type == u"shortcut"_s) {
        Action action;
        action.name = c.commandId;
        action.text = c.command;
        action.description = c.osdMessage;
        action.shortcut = appActionsModel()->getShortcut(action.name, QString());
        action.type = u"CustomAction"_s;
        appActionsModel()->appendCustomAction(action);
    }
}

void CustomCommandsModel::editCustomCommand(int row, const QString &command, const QString &osdMessage, const QString &type)
{
    Command &c = m_customCommands[row];
    c.command = command;
    c.osdMessage = osdMessage;
    c.type = type;

    QString groupName = c.commandId;
    m_customCommandsConfig->group(groupName).writeEntry("Command", command);
    m_customCommandsConfig->group(groupName).writeEntry("OsdMessage", osdMessage);
    m_customCommandsConfig->group(groupName).writeEntry("Type", type);
    m_customCommandsConfig->sync();

    Q_EMIT dataChanged(index(row, 0), index(row, 0));
    if (c.type == u"shortcut"_s) {
        appActionsModel()->editCustomAction(c.commandId, c.command, c.osdMessage);
    }
}

void CustomCommandsModel::toggleCustomCommand(const QString &groupName, int row, bool setOnStartup)
{
    auto group = m_customCommandsConfig->group(groupName);
    if (setOnStartup == group.readEntry("SetOnStartup", true)) {
        return;
    }
    Command &customCommand = m_customCommands[row];
    customCommand.setOnStartup = setOnStartup;

    group.writeEntry("SetOnStartup", setOnStartup);
    m_customCommandsConfig->sync();

    Q_EMIT dataChanged(index(row, 0), index(row, 0));
}

void CustomCommandsModel::deleteCustomCommand(const QString &groupName, int row)
{
    beginRemoveRows(QModelIndex(), row, row);
    m_customCommandsConfig->deleteGroup(groupName);
    m_customCommandsConfig->sync();
    m_customCommands.removeAt(row);
    endRemoveRows();

    KSharedConfig::Ptr config = KSharedConfig::openConfig(PathUtils::instance()->configFilePath());
    config->group(u"Shortcuts"_s).deleteEntry(groupName);
    config->sync();
}

ActionsModel *CustomCommandsModel::appActionsModel()
{
    return m_appActionsModel;
}

void CustomCommandsModel::setAppActionsModel(ActionsModel *_appActionsModel)
{
    if (m_appActionsModel == _appActionsModel) {
        return;
    }
    m_appActionsModel = _appActionsModel;
    Q_EMIT appActionsModelChanged();
}

#include "moc_customcommandsmodel.cpp"
