/*
 * SPDX-FileCopyrightText: 2022 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQml 2.15
import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.3

import org.kde.kirigami 2.20 as Kirigami
import org.kde.kquickcontrols 2.0
import org.kde.haruna 1.0
import Haruna.Components 1.0

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

        property int rowHeight: Math.ceil(Kirigami.Units.gridUnit * 2.4)

        model: proxyActionsModel
        implicitHeight: rowHeight * model.count
        delegate: ListItem {
            height: actionsListView.rowHeight
            text: model.text
            leading: Kirigami.Icon {
                source: model.icon
                width: Kirigami.Units.iconSizes.smallMedium
                height: Kirigami.Units.iconSizes.smallMedium
            }
            trailing: KeySequenceItem {
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
