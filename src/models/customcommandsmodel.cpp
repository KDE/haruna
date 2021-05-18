#include "customcommandsmodel.h"

#include "_debug.h"

#include <KConfigGroup>
#include <QCollator>

CustomCommandsModel::CustomCommandsModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_customCommandsConfig = KSharedConfig::openConfig("georgefb/haruna-custom-commands.conf",
                                                    KConfig::SimpleConfig);
    getCommands();
}

void CustomCommandsModel::getCommands()
{
    m_customCommands.clear();

    QStringList groups = m_customCommandsConfig->groupList();

    QCollator collator;
    collator.setNumericMode(true);
    std::sort(groups.begin(), groups.end(), collator);

    beginInsertRows(QModelIndex(), 0, groups.size());
    for (const QString &_group : qAsConst((groups))) {
        auto configGroup = m_customCommandsConfig->group(_group);
        Command p;
        p.commandId = _group;
        p.command = configGroup.readEntry("Command", QString());
        p.osdMessage = configGroup.readEntry("OsdMessage", QString()),
        p.type = configGroup.readEntry("Type", QString());
        m_customCommands << p;
    }
    endInsertRows();
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
    endMoveRows();
}

void CustomCommandsModel::saveCustomCommand(
        const QString &groupName,
        const QString &command,
        const QString &osdMessage,
        const QString &type)
{
    if (m_customCommandsConfig->group(groupName).exists()) {
        return;
    }
    m_customCommandsConfig->group(groupName).writeEntry(QStringLiteral("Command"), command);
    m_customCommandsConfig->group(groupName).writeEntry(QStringLiteral("OsdMessage"), osdMessage);
    m_customCommandsConfig->group(groupName).writeEntry(QStringLiteral("Type"), type);
    m_customCommandsConfig->sync();
}

ProxyCustomCommandsModel::ProxyCustomCommandsModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setDynamicSortFilter(true);
    setFilterRole(CustomCommandsModel::TypeRole);
    setFilterCaseSensitivity(Qt::CaseInsensitive);
    setFilterFixedString(QStringLiteral("shortcut"));
}
