/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import org.kde.kirigami 2.11 as Kirigami

Popup {
    id: root

    property string headerTitle
    property int buttonIndex: -1

    signal actionSelected(string actionName)

    implicitHeight: parent.height * 0.9
    implicitWidth: parent.width * 0.9
    modal: true
    anchors.centerIn: parent
    focus: true

    onOpened: {
        filterActionsField.text = ""
        filterActionsField.focus = true
    }

    onActionSelected: close()

    Action {
        shortcut: "ctrl+f"
        onTriggered: filterActionsField.forceActiveFocus(Qt.ShortcutFocusReason)
    }

    ColumnLayout {
        anchors.fill: parent

        Kirigami.Heading {
            text: root.headerTitle
        }

        Label {
            text: i18nc("@title", "Double click to set action")
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignTop
        }

        Kirigami.SearchField {
            id: filterActionsField

            focus: true
            onAccepted: proxyActionsModel.setNameFilter(text)
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignTop
            KeyNavigation.up: actionsListView
            KeyNavigation.down: clearActionButton
        }
        Button {
            id: clearActionButton

            Layout.fillWidth: true
            text: i18nc("@action:button", "Clear current action")
            KeyNavigation.up: filterActionsField
            KeyNavigation.down: actionsListView
            onClicked: actionSelected("")
            Keys.onEnterPressed: actionSelected("")
            Keys.onReturnPressed: actionSelected("")
        }

        ListView {
            id: actionsListView

            implicitHeight: 30 * model.count
            model: proxyActionsModel
            spacing: 1
            clip: true
            currentIndex: focus ? 0 : -1
            delegate: Kirigami.BasicListItem {
                height: 30
                width: root.width
                label: model.text
                reserveSpaceForIcon: false
                onDoubleClicked: actionSelected(model.name)
                Keys.onEnterPressed: actionSelected(model.name)
                Keys.onReturnPressed: actionSelected(model.name)
            }

            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignTop
            KeyNavigation.up: clearActionButton
            KeyNavigation.down: filterActionsField
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
}
