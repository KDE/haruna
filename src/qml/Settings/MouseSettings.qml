/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQml 2.12
import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

import org.kde.kirigami 2.11 as Kirigami
import org.kde.haruna 1.0
import Haruna.Components 1.0

SettingsBasePage {
    id: root

    hasHelp: false
    helpFile: ""

    ColumnLayout {
        id: content

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

            implicitHeight: delegateHeight * (mouseButtonsListView.count + 1)
            model: mouseActionsModel

            delegate: ListItem {
                id: delegate

                label: model.label
                subtitle: MouseSettings[model.key]
                          ? appActions[MouseSettings[model.key]].text
                          : i18nc("@label", "No action set")
                icon: MouseSettings[model.key] ? "checkmark" : ""
                reserveSpaceForIcon: true
                width: content.width
                highlighted: false

                trailing: ToolButton {
                    icon.name: "edit-clear-all"
                    onClicked: {
                        MouseSettings[model.key] = ""
                        MouseSettings.save()
                    }

                    ToolTip {
                        text: i18nc("@info:tooltip", "Clear action")
                    }
                }

                onClicked: openSelectActionPopup()
                Component.onCompleted: mouseButtonsListView.delegateHeight = height

                Connections {
                    target: selectActionPopup
                    onActionSelected: {
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
