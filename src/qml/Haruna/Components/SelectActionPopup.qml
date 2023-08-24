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

    property string title: ""
    property string subtitle: ""
    property int buttonIndex: -1

    signal actionSelected(string actionName)

    implicitHeight: parent.height * 0.9
    implicitWidth: parent.width * 0.9
    modal: true
    anchors.centerIn: parent
    focus: true

    onOpened: {
        actionsListView.positionViewAtBeginning()
        filterActionsField.text = ""
        filterActionsField.focus = true
    }

    onActionSelected: close()

    ColumnLayout {
        anchors.fill: parent

        Kirigami.Heading {
            text: root.title
            visible: text !== ""
        }

        Label {
            text: root.subtitle
            visible: text !== ""
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
            KeyNavigation.down: actionsListView
            KeyNavigation.tab: actionsListView
        }

        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignTop

            ListView {
                id: actionsListView

                model: proxyActionsModel
                spacing: 1
                clip: true
                currentIndex: focus ? 0 : -1
                delegate: ListItem {
                    label: model.text
                    trailing: Label {
                        text: model.shortcut
                        opacity: 0.7
                    }
                    reserveSpaceForIcon: false
                    onClicked: actionSelected(model.name)
                    Keys.onEnterPressed: actionSelected(model.name)
                    Keys.onReturnPressed: actionSelected(model.name)
                }

                KeyNavigation.up: filterActionsField
                KeyNavigation.down: filterActionsField
                Keys.onPressed: {
                    if (event.key === Qt.Key_End) {
                        actionsListView.currentIndex = actionsListView.count - 1
                        actionsListView.positionViewAtEnd()
                    }
                    if (event.key === Qt.Key_Home) {
                        actionsListView.currentIndex = 0
                        actionsListView.positionViewAtBeginning()
                    }
                }
            }
        }
    }
}
