/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQml
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import org.kde.kirigami as Kirigami
import org.kde.haruna
import org.kde.haruna.settings

SettingsBasePage {
    id: root

    hasHelp: false
    helpFile: ""

    ColumnLayout {
        id: content

        anchors.fill: parent
        spacing: Kirigami.Units.largeSpacing

        ListModel {
            id: mouseActionsModel

            ListElement {
                label: "Left"
                key: "left"
            }
            ListElement {
                label: "Left double click"
                key: "leftx2"
            }
            ListElement {
                label: "Right"
                key: "right"
            }
            ListElement {
                label: "Right double click"
                key: "rightx2"
            }
            ListElement {
                label: "Middle"
                key: "middle"
            }
            ListElement {
                label: "Middle double click"
                key: "middlex2"
            }
            ListElement {
                label: "Scroll up"
                key: "scrollUp"
            }
            ListElement {
                label: "Scroll down"
                key: "scrollDown"
            }
        }

        ListView {
            id: mouseButtonsListView

            property int delegateHeight

            model: mouseActionsModel
            delegate: ItemDelegate {
                id: delegate

                width: content.width
                highlighted: false

                contentItem: RowLayout {
                    Kirigami.IconTitleSubtitle {
                        title: model.label
                        subtitle: MouseSettings[model.key]
                                  ? appActions[MouseSettings[model.key]].text
                                  : i18nc("@label", "No action set")
                        icon.name: MouseSettings[model.key] ? "checkmark" : ""

                        Layout.fillWidth: true
                    }
                    ToolButton {
                        visible: MouseSettings[model.key]
                        icon.name: "edit-clear-all"
                        onClicked: {
                            MouseSettings[model.key] = ""
                            MouseSettings.save()
                        }

                        ToolTip {
                            text: i18nc("@info:tooltip", "Clear action")
                        }
                    }
                }

                onClicked: openSelectActionPopup()
                Component.onCompleted: mouseButtonsListView.delegateHeight = height

                Connections {
                    target: selectActionPopup
                    function onActionSelected(actionName) {
                        if (selectActionPopup.buttonIndex === model.index) {
                            MouseSettings[model.key] = actionName
                            MouseSettings.save()
                        }
                    }
                }

                function openSelectActionPopup() {
                    selectActionPopup.buttonIndex = model.index
                    selectActionPopup.title = model.label
                    selectActionPopup.open()
                }
            }

            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        Item {
            width: Kirigami.Units.gridUnit
            height: Kirigami.Units.gridUnit
        }

        SelectActionPopup {
            id: selectActionPopup

            subtitle: i18nc("@title", "Double click to set action")
        }
    }
}
