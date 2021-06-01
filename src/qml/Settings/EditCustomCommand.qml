/*
 * SPDX-FileCopyrightText: 2021 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

import org.kde.kirigami 2.11 as Kirigami
import org.kde.haruna 1.0
import Haruna.Components 1.0

SettingsBasePage {
    id: root

    property string command: ""
    property string osdMessage: ""
    property string type: "shortcut"
    property string commandId: ""
    property int index: -1
    property int mode: EditCustomCommand.Mode.Create

    enum Mode {
        Create = 0,
        Edit
    }

    hasHelp: true
    helpFile: ":/CustomCommandsSettings.html"

    Action {
        shortcut: "esc"
        onTriggered: applicationWindow().pageStack.replace("qrc:/CustomCommands.qml")
    }

    GridLayout {
        columns: 2

        Label {
            text: i18n("Command")
            Layout.alignment: Qt.AlignRight
        }

        TextField {
            id: commandTextField

            text: root.command
            placeholderText: "add volume +10"
            Layout.fillWidth: true
            Component.onCompleted: forceActiveFocus()
        }

        Label {
            text: i18n("OSD Message")
            Layout.alignment: Qt.AlignRight
        }

        TextField {
            id: osdMessageTextField

            text: root.osdMessage
            enabled: typeGroup.checkedButton.optionName === "shortcut"
            placeholderText: "Filename: ${filename}"
            Layout.fillWidth: true
        }

        Label {
            text: i18n("Type")
            Layout.alignment: Qt.AlignRight | Qt.AlignTop
        }

        ButtonGroup {
            id: typeGroup

            buttons: typeGroupItems.children
        }

        Column {
            id: typeGroupItems

            spacing: Kirigami.Units.largeSpacing

            RadioButton {
                property string optionName: "shortcut"

                checked: optionName === root.type
                text: i18n("Keyboard shortcut")
            }

            RadioButton {
                property string optionName: "startup"

                checked: optionName === root.type
                text: i18n("Run at startup")
            }
        }
    }

    footer: ToolBar {
        RowLayout {
            anchors.fill: parent

            ToolButton {
                text: i18n("Delete")
                icon.name: "delete"
                visible: root.mode === EditCustomCommand.Mode.Edit
                onClicked: {
                    customCommandsModel.deleteCustomCommand(root.commandId, root.index)
                    applicationWindow().pageStack.replace("qrc:/CustomCommands.qml")
                }
            }

            Item {

                Layout.fillWidth: true
            }

            ToolButton {
                text: i18n("Cancel")
                icon.name: "dialog-cancel"
                onClicked: applicationWindow().pageStack.replace("qrc:/CustomCommands.qml")
                Layout.alignment: Qt.AlignRight
            }

            ToolButton {
                text: i18n("&Save")
                icon.name: "document-save"
                enabled: commandTextField.text !== ""
                onClicked: saveCustomCommand()

                Layout.alignment: Qt.AlignRight

                function saveCustomCommand() {
                    if (commandTextField.text === "") {
                        return
                    }
                    if (typeGroup.checkedButton.optionName === "shortcut") {
                        // creates action and adds it to the action collection
                        // so that its shortcut can be set by the user
                        app.createUserAction(commandTextField.text)
                    } else {
                        // execute the user command
                        mpv.userCommand(commandTextField.text)
                    }
                    switch (root.mode) {
                    case EditCustomCommand.Mode.Create:
                        // save new command to config file
                        customCommandsModel.saveCustomCommand(commandTextField.text,
                                                              osdMessageTextField.text,
                                                              typeGroup.checkedButton.optionName)
                        break
                    case EditCustomCommand.Mode.Edit:
                        // update existing command
                        customCommandsModel.editCustomCommand(root.index,
                                                              commandTextField.text,
                                                              osdMessageTextField.text,
                                                              typeGroup.checkedButton.optionName)
                        break
                    }
                    applicationWindow().pageStack.replace("qrc:/CustomCommands.qml")
                }

            }
        }
    }
}
