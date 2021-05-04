#include "custompropertiesmodel.h"

#include "_debug.h"

#include <KConfigGroup>

CustomPropertiesModel::CustomPropertiesModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_customPropsConfig = KSharedConfig::openConfig("georgefb/haruna-custom-properties.conf",
                                                    KConfig::SimpleConfig);
    const QStringList groups = m_customPropsConfig->groupList();

    beginInsertRows(QModelIndex(), 0, groups.size());
    for (const QString &_group : groups) {
        auto configGroup = m_customPropsConfig->group(_group);
        Property p;
        p.command = configGroup.readEntry("Command", QString());
        p.osdMessage = configGroup.readEntry("OsdMessage", QString());
        p.shortcut = configGroup.readEntry("Shortcut", QString());
        p.setAtStartUp = configGroup.readEntry("SetAtStartUp", false);
        m_customProperties << p;
    }
    endInsertRows();
}

int CustomPropertiesModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_customProperties.size();
}

QVariant CustomPropertiesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    Property prop = m_customProperties[index.row()];

    switch (role) {
    case CommandRole:
        return QVariant(prop.command);
    case OsdMessageRole:
        return QVariant(prop.osdMessage);
    case ShortcutRole:
        return QVariant(prop.shortcut);
    case SetAtStartUp:
        return QVariant(prop.setAtStartUp);
    }

    return QVariant();
}

QHash<int, QByteArray> CustomPropertiesModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[CommandRole] = "command";
    roles[OsdMessageRole] = "osdMessage";
    roles[ShortcutRole] = "shortcut";
    roles[SetAtStartUp] = "setAtStartUp";
    return roles;
}
