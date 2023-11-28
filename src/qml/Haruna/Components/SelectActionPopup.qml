/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

import org.kde.haruna.models

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
        filterActionsField.selectAll()
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
            onAccepted: selectActionModel.setNameFilter(text)
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignTop
            KeyNavigation.up: actionsListView
            KeyNavigation.down: actionsListView
            KeyNavigation.tab: actionsListView
            Keys.onReturnPressed: {
                actionSelected(actionsListView.currentItem.actionName)
            }
            Keys.onEnterPressed: {
                actionSelected(actionsListView.currentItem.actionName)
            }
        }

        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignTop

            ListView {
                id: actionsListView

                model: ProxyActionsModel {
                    id: selectActionModel

                    sourceModel: actionsModel
                }

                spacing: 1
                clip: true
                delegate: ItemDelegate {
                    property string actionName: model.name

                    width: actionsListView.width

                    contentItem: RowLayout {
                        Label {
                            text: model.text

                            Layout.fillWidth: true
                        }

                        Label {
                            text: model.shortcut
                            opacity: 0.7
                        }
                    }
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
