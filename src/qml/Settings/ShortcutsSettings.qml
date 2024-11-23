/*
 * SPDX-FileCopyrightText: 2022 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQml
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Dialogs

import org.kde.kirigami as Kirigami
import org.kde.kquickcontrols
import org.kde.haruna

SettingsBasePage {
    id: root

    required property ProxyActionsModel m_proxyActionsModel

    onVisibleChanged: {
        if (!visible) {
            root.m_proxyActionsModel.setNameFilter("")
        }
    }

    header: ToolBar {
        width: parent.width

        RowLayout {
            anchors.fill: parent

            Kirigami.SearchField {
                id: searchField

                focus: true
                onAccepted: root.m_proxyActionsModel.setNameFilter(text)
                Layout.fillWidth: true
                KeyNavigation.up: actionsListView
                KeyNavigation.down: actionsListView
                Component.onCompleted: forceActiveFocus()
            }
        }
    }
    ListView {
        id: actionsListView

        model: root.m_proxyActionsModel
        reuseItems: true
        delegate: ItemDelegate {
            id: delegate

            required property int index
            required property string actionName
            required property string actionText
            required property string actionDescription
            required property string actionShortcut
            required property string actionIcon
            required property string actionType

            width: actionsListView.width

            contentItem: RowLayout {
                Kirigami.IconTitleSubtitle {
                    title: delegate.actionText
                    icon.name: delegate.actionIcon

                    Layout.fillWidth: true
                }

                KeySequenceItem {
                    checkForConflictsAgainst: ShortcutType.None
                    modifierlessAllowed: true
                    keySequence: delegate.actionShortcut

                    onKeySequenceModified: {
                        if (keySequence.toString() === "" ) {
                            return
                        }

                        if (keySequence.toString() !== delegate.actionShortcut) {
                            if (!root.m_proxyActionsModel.saveShortcut(delegate.index, keySequence)) {
                                keySequence = delegate.actionShortcut
                            }
                        }
                    }
                }
            }
        }
        KeyNavigation.down: searchField
        KeyNavigation.up: searchField
        Keys.onPressed: function (event) {
            if (event.key === Qt.Key_End) {
                actionsListView.currentIndex = actionsListView.count - 1
                actionsListView.positionViewAtIndex(actionsListView.currentIndex,ListView.Center)
            }
            if (event.key === Qt.Key_Home) {
                actionsListView.currentIndex = 0
                actionsListView.positionViewAtIndex(actionsListView.currentIndex,ListView.Center)
            }
        }
    }
}
