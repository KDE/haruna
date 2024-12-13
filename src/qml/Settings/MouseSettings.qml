/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQml
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import org.kde.kirigami as Kirigami
import org.kde.haruna
import org.kde.haruna.settings

SettingsBasePage {
    id: root

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

                required property int index
                required property string label
                required property string key

                width: content.width
                highlighted: false

                contentItem: RowLayout {
                    Kirigami.IconTitleSubtitle {
                        title: delegate.label
                        subtitle: MouseSettings[delegate.key]
                                  ? appActions[MouseSettings[delegate.key]].text
                                  : i18nc("@label", "No action set")
                        icon.name: MouseSettings[delegate.key] ? "checkmark" : ""

                        Layout.fillWidth: true
                    }
                    ToolButton {
                        visible: MouseSettings[delegate.key]
                        icon.name: "edit-clear-all"
                        onClicked: {
                            MouseSettings[delegate.key] = ""
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
                        if (selectActionPopup.buttonIndex === delegate.index) {
                            MouseSettings[delegate.key] = actionName
                            MouseSettings.save()
                        }
                    }
                }

                function openSelectActionPopup() : void {
                    selectActionPopup.buttonIndex = delegate.index
                    selectActionPopup.title = delegate.label
                    selectActionPopup.open()
                }
            }

            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        Item {
            Layout.preferredWidth: Kirigami.Units.gridUnit
            Layout.preferredHeight: Kirigami.Units.gridUnit
        }

        SelectActionPopup {
            id: selectActionPopup

            subtitle: i18nc("@title", "Double click to set action")
        }
    }
}
