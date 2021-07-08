#include "customcommandsmodel.h"

#include "_debug.h"

#include <KConfigGroup>
#include <global.h>

CustomCommandsModel::CustomCommandsModel(QObject *parent)
    : QAbstractListModel(parent)
{
    QString ccConfig = Global::instance()->appConfigFilePath(Global::ConfigFile::CustomCommands);
    m_customCommandsConfig = KSharedConfig::openConfig(ccConfig, KConfig::SimpleConfig);
    QStringList groups = m_customCommandsConfig->groupList();

    beginInsertRows(QModelIndex(), 0, groups.size());
    for (const QString &groupName : qAsConst((groups))) {
        auto configGroup = m_customCommandsConfig->group(groupName);
        auto c = new Command();
        c->commandId = groupName;
        c->command = configGroup.readEntry("Command", QString());
        c->osdMessage = configGroup.readEntry("OsdMessage", QString());
        c->type = configGroup.readEntry("Type", QString());
        c->order = configGroup.readEntry("Order", 0);
        m_customCommands << c;
    }

    std::sort(m_customCommands.begin(), m_customCommands.end(), [=](Command *c1, Command *c2) {
        return c1->order < c2->order;
    });

    endInsertRows();

    connect(this, &QAbstractListModel::rowsMoved, this, [=]() {
        for (int i = 0; i < m_customCommands.size(); ++i) {
            auto configGroup = m_customCommandsConfig->group(m_customCommands[i]->commandId);
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

    Command *command = m_customCommands[index.row()];

    switch (role) {
    case CommandIdRole:
        return QVariant(command->commandId);
    case CommandRole:
        return QVariant(command->command);
    case OsdMessageRole:
        return QVariant(command->osdMessage);
    case TypeRole:
        return QVariant(command->type);
    }

    return QVariant();
}

QHash<int, QByteArray> CustomCommandsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[CommandIdRole] = "commandId";
    roles[OsdMessageRole] = "osdMessage";
    roles[CommandRole] = "command";
    roles[TypeRole] = "type";
    return roles;
}

void CustomCommandsModel::moveRows(int oldIndex, int newIndex)
{
    if (oldIndex < newIndex) {
        beginMoveRows(QModelIndex(), oldIndex, oldIndex, QModelIndex(), newIndex + 1);
    } else {
        beginMoveRows(QModelIndex(), oldIndex, oldIndex, QModelIndex(), newIndex);
    }
    Command *c = m_customCommands.takeAt(oldIndex);
    m_customCommands.insert(newIndex, c);
    endMoveRows();
}

void CustomCommandsModel::saveCustomCommand(const QString &command, const QString &osdMessage, const QString &type)
{
    int counter = m_customCommandsConfig->group(QString()).readEntry("Counter", 0);
    const QString &groupName = QString("Command_%1").arg(counter);

    if (m_customCommandsConfig->group(groupName).exists()) {
        return;
    }

    m_customCommandsConfig->group(groupName).writeEntry(QStringLiteral("Command"), command);
    m_customCommandsConfig->group(groupName).writeEntry(QStringLiteral("OsdMessage"), osdMessage);
    m_customCommandsConfig->group(groupName).writeEntry(QStringLiteral("Type"), type);
    m_customCommandsConfig->group(groupName).writeEntry(QStringLiteral("Order"), rowCount());
    m_customCommandsConfig->group(QString()).writeEntry(QStringLiteral("Counter"), counter + 1);
    m_customCommandsConfig->sync();

    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    auto configGroup = m_customCommandsConfig->group(groupName);
    auto p = new Command();
    p->commandId = groupName;
    p->command = configGroup.readEntry("Command", QString());
    p->osdMessage = configGroup.readEntry("OsdMessage", QString()),
    p->type = configGroup.readEntry("Type", QString());
    m_customCommands << p;
    endInsertRows();
}

void CustomCommandsModel::editCustomCommand(int row, const QString &command,
                                            const QString &osdMessage, const QString &type)
{
    auto customCommand = m_customCommands[row];
    customCommand->command = command;
    customCommand->osdMessage = osdMessage;
    customCommand->type = type;

    QString groupName = customCommand->commandId;
    m_customCommandsConfig->group(groupName).writeEntry(QStringLiteral("Command"), command);
    m_customCommandsConfig->group(groupName).writeEntry(QStringLiteral("OsdMessage"), osdMessage);
    m_customCommandsConfig->group(groupName).writeEntry(QStringLiteral("Type"), type);
    m_customCommandsConfig->sync();

    emit dataChanged(index(row, 0), index(row, 0));
}

void CustomCommandsModel::deleteCustomCommand(const QString &groupName, int row)
{
    beginRemoveRows(QModelIndex(), row, row);
    m_customCommandsConfig->deleteGroup(groupName);
    m_customCommandsConfig->sync();
    m_customCommands.removeAt(row);
    endRemoveRows();

    KSharedConfig::Ptr config = KSharedConfig::openConfig(Global::instance()->appConfigFilePath());
    config->group("Shortcuts").deleteEntry(groupName);
    config->sync();
}

ProxyCustomCommandsModel::ProxyCustomCommandsModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setDynamicSortFilter(true);
    setFilterRole(CustomCommandsModel::TypeRole);
    setFilterCaseSensitivity(Qt::CaseInsensitive);
    setFilterFixedString(QStringLiteral("shortcut"));
}
