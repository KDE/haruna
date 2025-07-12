/*
 * SPDX-FileCopyrightText: 2025 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef MOUSEACTIONSMODEL_H
#define MOUSEACTIONSMODEL_H

#include <QAbstractListModel>
#include <QtQmlIntegration/qqmlintegration.h>

#include <KSharedConfig>

using namespace Qt::StringLiterals;

struct MouseAction {
    QString actionName;
    int mouseButton{Qt::LeftButton};
    Qt::KeyboardModifier modifier{Qt::NoModifier};
    bool isDoubleClick{false};
};

inline QDebug operator<<(QDebug debug, const MouseAction &ba)
{
    QDebugStateSaver saver(debug);
    // clang-format off
    debug.nospace() << "\nButtonAction {"
                    << "\n\tactionName: "    << ba.actionName
                    << "\n\tmouseButton: "   << ba.mouseButton
                    << "\n\tmodifier: "      << ba.modifier
                    << "\n\tisDoubleClick: " << ba.isDoubleClick
                    << "\n}";
    // clang-format on
    return debug;
}

class MouseActionsModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit MouseActionsModel(QObject *parent = nullptr);

    enum Roles {
        ActionName,
        Button,
        Modifier,
        IsDoubleClick,
    };
    Q_ENUM(Roles)

    enum MouseButton {
        NoButton = Qt::MouseButton::NoButton,
        Left = Qt::MouseButton::LeftButton,
        Middle = Qt::MouseButton::MiddleButton,
        Right = Qt::MouseButton::RightButton,
        Forward = Qt::MouseButton::ForwardButton,
        Back = Qt::MouseButton::BackButton,
        ScrollUp = 1001,
        ScrollDown = 1002,
    };
    Q_ENUM(MouseButton)

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

public Q_SLOTS:
    void addAction(const QString &actionName, const QString &button, const QString &modifier, bool isDoubleClick);
    void removeAction(uint row);
    QString getAction(MouseActionsModel::MouseButton button, Qt::KeyboardModifier modifier = Qt::NoModifier, bool isDoubleClick = false);
    bool actionExists(const QString &button, const QString &modifier, bool isDoubleClick);
    MouseActionsModel::MouseButton stringToMouseButton(const QString &token);
    Qt::KeyboardModifier stringToModifier(const QString &token);

private:
    QString mouseButtonToString(MouseButton button) const;
    QString modifierToString(Qt::KeyboardModifier modifier) const;
    QString configKey(MouseButton button, Qt::KeyboardModifier modifier, bool isDoubleClick);
    QString configKey(MouseAction ba);
    bool isDefaultAction(const QString &actionKey);
    QString getDefaultAction(const QString &actionKey);
    QList<MouseAction> m_data;
    KSharedConfig::Ptr m_config;
    // clang-format off
    const QMap<QString, QString> m_defaultKeys{
        {u"Left"_s,       u"playPauseAction"_s},
        {u"Leftx2"_s,     u"toggleFullscreenAction"_s},
        {u"Right"_s,      u"openContextMenuAction"_s},
        {u"Middle"_s,     u"muteAction"_s},
        {u"Forward"_s,    u"seekForwardSmallAction"_s},
        {u"Back"_s,       u"seekBackwardSmallAction"_s},
        {u"ScrollUp"_s,   u"volumeUpAction"_s},
        {u"ScrollDown"_s, u"volumeDownAction"_s},
    };
    // clang-format on
};

#endif // MOUSEACTIONSMODEL_H
