/*
 * SPDX-FileCopyrightText: 2025 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "mouseactionsmodel.h"

#include <KConfigGroup>

#include "pathutils.h"

MouseActionsModel::MouseActionsModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_config(KSharedConfig::openConfig(PathUtils::instance()->configFilePath()))
{
    auto configGroup{m_config->group(u"Mouse"_s)};
    const auto keys{configGroup.keyList()};

    auto buttonActionFromConfigKey = [this, configGroup](const QString &key) -> MouseAction {
        MouseAction ba;
        QString token;
        for (int i = 0; i < key.length(); ++i) {
            auto c = key.at(i);
            if (c == u"_"_s) {
                ba.modifier = stringToModifier(token);
                token.clear();
                continue;
            }
            if (c == u"x"_s) {
                ba.mouseButton = stringToMouseButton(token);
                ba.isDoubleClick = true;
                break;
            }
            token.append(c);
            if (i == key.length() - 1) {
                ba.mouseButton = stringToMouseButton(token);
            }
        }
        ba.actionName = configGroup.readEntry(key);

        return ba;
    };

    for (const auto &key : keys) {
        MouseAction btnAction = buttonActionFromConfigKey(key);
        if (btnAction.actionName.isEmpty()) {
            continue;
        }
        m_data.append(btnAction);
    }

    const auto defaultKeys = m_defaultKeys.keys();
    for (const auto &key : defaultKeys) {
        if (configGroup.hasKey(key)) {
            continue;
        }
        MouseAction btnAction = buttonActionFromConfigKey(key);
        btnAction.actionName = m_defaultKeys.value(key);
        m_data.append(btnAction);
    }
}

int MouseActionsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return m_data.count();
}

QVariant MouseActionsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    const auto item = m_data.at(index.row());

    switch (role) {
    case MouseActionRole:
        return QVariant::fromValue(item);
        break;
    case ActionName:
        return item.actionName;
        break;
    case Button:
        return mouseButtonToString(static_cast<MouseButton>(item.mouseButton));
        break;
    case Modifier:
        return modifierToString(static_cast<Qt::KeyboardModifier>(item.modifier));
        break;
    case IsDoubleClick:
        return item.isDoubleClick;
        break;
    }

    return QVariant();
}

QHash<int, QByteArray> MouseActionsModel::roleNames() const
{
    static QHash<int, QByteArray> roles{
        {MouseActionRole, QByteArrayLiteral("mouseAction")},
        {ActionName, QByteArrayLiteral("actionName")},
        {Button, QByteArrayLiteral("button")},
        {Modifier, QByteArrayLiteral("modifier")},
        {IsDoubleClick, QByteArrayLiteral("isDoubleClick")},
    };

    return roles;
}

void MouseActionsModel::addAction(const QString &actionName, const QString &button, const QString &modifier, bool isDoubleClick)
{
    MouseAction btnAction;
    btnAction.actionName = actionName;
    btnAction.mouseButton = stringToMouseButton(button);
    btnAction.modifier = stringToModifier(modifier);
    btnAction.isDoubleClick = isDoubleClick;

    beginInsertRows({}, rowCount(), rowCount());
    m_data.append(btnAction);
    endInsertRows();

    auto configGroup{m_config->group(u"Mouse"_s)};
    configGroup.writeEntry(configKey(btnAction), actionName);
    configGroup.sync();
}

void MouseActionsModel::editAction(MouseAction mouseAction)
{
    auto key = configKey(mouseAction);
    for (int i = 0; i < rowCount(); ++i) {
        auto &a = m_data[i];
        auto currentKey = configKey(a);
        if (currentKey == key) {
            a.actionName = mouseAction.actionName;

            auto configGroup{m_config->group(u"Mouse"_s)};
            configGroup.writeEntry(key, a.actionName);
            configGroup.sync();

            Q_EMIT dataChanged(index(i, 0), index(i, 0));
            return;
        }
    }
}

void MouseActionsModel::removeAction(uint row)
{
    beginRemoveRows({}, row, row);
    const auto item = m_data.takeAt(row);
    endRemoveRows();

    QString actionKey{configKey(item)};
    auto configGroup{m_config->group(u"Mouse"_s)};
    if (isDefaultAction(actionKey)) {
        // default mouse button actions are set to an empty string
        // since removing them would trigger their default action
        configGroup.writeEntry(actionKey, QString{});
    } else {
        configGroup.deleteEntry(configKey(item));
    }
    configGroup.sync();
}

QString MouseActionsModel::getAction(MouseActionsModel::MouseButton button, Qt::KeyboardModifier modifier, bool isDoubleClick)
{
    auto configGroup{m_config->group(u"Mouse"_s)};
    QString actionKey{configKey(button, modifier, isDoubleClick)};
    auto actionName = configGroup.readEntry(actionKey);

    auto hasKey{configGroup.hasKey(actionKey)};
    // if the requested key is not in the config it might be a default one
    if (!hasKey && isDefaultAction(actionKey)) {
        return getDefaultAction(actionKey);
    }

    return actionName;
}

bool MouseActionsModel::actionExists(const QString &button, const QString &modifier, bool isDoubleClick)
{
    auto configGroup{m_config->group(u"Mouse"_s)};
    QString actionKey{configKey(stringToMouseButton(button), stringToModifier(modifier), isDoubleClick)};
    auto hasKey{configGroup.hasKey(actionKey)};
    auto exists{false};
    if (hasKey) {
        auto value = configGroup.readEntry(actionKey);
        auto hasValue{!value.isEmpty()};
        exists = hasValue;
    } else {
        exists = m_defaultKeys.contains(actionKey);
    }
    return exists;
}

bool MouseActionsModel::isDefaultAction(const QString &actionKey)
{
    return m_defaultKeys.contains(actionKey);
}

QString MouseActionsModel::getDefaultAction(const QString &actionKey)
{
    return m_defaultKeys.value(actionKey);
}

MouseActionsModel::MouseButton MouseActionsModel::stringToMouseButton(const QString &token)
{
    if (token == u"Left"_s) {
        return MouseButton::Left;
    } else if (token == u"Right"_s) {
        return MouseButton::Right;
    } else if (token == u"Middle"_s) {
        return MouseButton::Middle;
    } else if (token == u"Forward"_s) {
        return MouseButton::Forward;
    } else if (token == u"Back"_s) {
        return MouseButton::Back;
    } else if (token == u"ScrollUp"_s) {
        return MouseButton::ScrollUp;
    } else if (token == u"ScrollDown"_s) {
        return MouseButton::ScrollDown;
    } else {
        return MouseButton::NoButton;
    }
}

QString MouseActionsModel::mouseButtonToString(MouseButton button) const
{
    switch (button) {
    case MouseButton::Left:
        return u"Left"_s;
    case MouseButton::Right:
        return u"Right"_s;
    case MouseButton::Middle:
        return u"Middle"_s;
    case MouseButton::Forward:
        return u"Forward"_s;
    case MouseButton::Back:
        return u"Back"_s;
    case MouseButton::ScrollUp:
        return u"ScrollUp"_s;
    case MouseButton::ScrollDown:
        return u"ScrollDown"_s;
    case NoButton:
        return {};
    }

    return {};
}

Qt::KeyboardModifier MouseActionsModel::stringToModifier(const QString &token)
{
    if (token == u"Control"_s) {
        return Qt::ControlModifier;
    } else if (token == u"Shift"_s) {
        return Qt::ShiftModifier;
    } else if (token == u"Alt"_s) {
        return Qt::AltModifier;
    } else if (token == u"Meta"_s) {
        return Qt::MetaModifier;
    } else {
        return Qt::NoModifier;
    }
}

QString MouseActionsModel::modifierToString(Qt::KeyboardModifier modifier) const
{
    switch (modifier) {
    case Qt::KeyboardModifier::ControlModifier:
        return u"Control"_s;
    case Qt::KeyboardModifier::ShiftModifier:
        return u"Shift"_s;
    case Qt::KeyboardModifier::AltModifier:
        return u"Alt"_s;
    case Qt::KeyboardModifier::MetaModifier:
        return u"Meta"_s;
    case Qt::NoModifier:
    case Qt::KeypadModifier:
    case Qt::GroupSwitchModifier:
    case Qt::KeyboardModifierMask:
        return {};
    }

    return {};
}

QString MouseActionsModel::configKey(MouseButton button, Qt::KeyboardModifier modifier, bool isDoubleClick)
{
    QString modifierString{modifierToString(modifier)};
    if (!modifierString.isEmpty()) {
        modifierString.append(u"_"_s);
    }

    QString buttonString{mouseButtonToString(static_cast<MouseButton>(button))};
    QString isDoubleClickString;
    if (isDoubleClick) {
        isDoubleClickString = u"x2"_s;
    }

    return modifierString + buttonString + isDoubleClickString;
};

QString MouseActionsModel::configKey(MouseAction ba)
{
    return configKey(static_cast<MouseButton>(ba.mouseButton), static_cast<Qt::KeyboardModifier>(ba.modifier), ba.isDoubleClick);
};
