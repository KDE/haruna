/*
 * SPDX-FileCopyrightText: 2022 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQml
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Dialogs

import org.kde.kirigami as Kirigami
import org.kde.kquickcontrols
import org.kde.haruna
import Haruna.Components

SettingsBasePage {
    id: root

    hasHelp: false
    helpFile: ""

    onVisibleChanged: {
        if (!visible) {
            proxyActionsModel.setNameFilter("")
        }
    }

    header: ToolBar {
        width: parent.width

        RowLayout {
            anchors.fill: parent

            Kirigami.SearchField {
                id: searchField

                focus: true
                onAccepted: proxyActionsModel.setNameFilter(text)
                Layout.fillWidth: true
                KeyNavigation.up: actionsListView
                KeyNavigation.down: actionsListView
                Component.onCompleted: forceActiveFocus()
            }
        }
    }
    ListView {
        id: actionsListView

        model: proxyActionsModel

        delegate: ItemDelegate {
            width: actionsListView.width

            contentItem: RowLayout {
                Kirigami.IconTitleSubtitle {
                    title: model.text
                    icon.name: model.icon

                    Layout.fillWidth: true
                }

                KeySequenceItem {
                    checkForConflictsAgainst: ShortcutType.None
                    modifierlessAllowed: true
                    keySequence: model.shortcut

                    onKeySequenceChanged: {
                        if (keySequence !== model.shortcut) {
                            if (!proxyActionsModel.saveShortcut(model.index, keySequence)) {
                                keySequence = model.shortcut
                            }
                        }
                    }
                }
            }
        }
        KeyNavigation.down: searchField
        KeyNavigation.up: searchField
        Keys.onPressed: {
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
