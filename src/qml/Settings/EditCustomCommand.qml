/*
 * SPDX-FileCopyrightText: 2021 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import org.kde.kirigami as Kirigami
import org.kde.haruna

SettingsBasePage {
    id: root

    required property CustomCommandsModel m_customCommandsModel

    property string settingsPath: "qrc:/qt/qml/org/kde/haruna/qml/Settings"
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

    Action {
        shortcut: "esc"
        onTriggered: applicationWindow().pageStack.replace(`${root.settingsPath}/CustomCommandsSettings.qml`,
                                                           {m_customCommandsModel: root.m_customCommandsModel})
    }

    GridLayout {
        columns: 2

        Label {
            text: i18nc("@label:textbox", "Command")
            Layout.alignment: Qt.AlignRight
        }

        RowLayout {
            TextField {
                id: commandTextField

                text: root.command
                placeholderText: "add volume +10"
                Layout.fillWidth: true
                Component.onCompleted: forceActiveFocus()
            }

            ToolTipButton {
                toolTipText: i18nc("@info:tooltip",
                                   "What mpv command to run, " +
                                   "see <a href=\"https://mpv.io/manual/stable/#list-of-input-commands\">MPV Commands</a> " +
                                   "for a list of available commands.<br>" +
                                   "Not all commands will work as some are specific for mpv.<br>" +
                                   "Most useful are the commands to manipulate properties, " +
                                   "like <strong>set</strong>, <strong>add</strong>, <strong>cycle</strong>.<br>" +
                                   "<strong>set</strong>: sets the value of a property <strong>set volume-max 150</strong>.<br>" +
                                   "<strong>add</strong>: increases/decreases the value of a property <strong>add volume 15</strong>, " +
                                   "increases volume by 15. To decrease the value use <strong>add volume -15</strong>.<br>" +
                                   "<strong>cycle</strong>: cycle through the values of a property <strong>cycle aid up</strong>.")
                toolTipWidth: 450

                Layout.preferredHeight: Kirigami.Units.iconSizes.medium
            }
        }

        Label {
            text: i18nc("@label:textbox", "OSD Message")
            Layout.alignment: Qt.AlignRight
        }

        RowLayout {
            TextField {
                id: osdMessageTextField

                text: root.osdMessage
                enabled: typeGroup.checkedButton.optionName === "shortcut"
                placeholderText: i18nc("placeholder text; %1 is an example of how to display an mpv property", "Filename: %1", "${filename}")
                Layout.fillWidth: true
            }

            ToolTipButton {
                toolTipText: i18nc("@info:tooltip",
                                   "An OSD message to display when triggering an action type custom command.<br>" +
                                   "Use ${property_name} to display the value of a property <strong>Volume: ${volume}</strong>")
                toolTipWidth: 450

                Layout.preferredHeight: Kirigami.Units.iconSizes.medium
            }
        }

        Label {
            text: i18nc("@label", "Type")
            visible: root.mode === EditCustomCommand.Mode.Create
            Layout.alignment: Qt.AlignRight | Qt.AlignTop
        }

        ButtonGroup {
            id: typeGroup

            buttons: typeGroupItems.children
        }

        Column {
            id: typeGroupItems

            spacing: Kirigami.Units.largeSpacing
            visible: root.mode === EditCustomCommand.Mode.Create

            RadioButton {
                property string optionName: "shortcut"

                checked: optionName === root.type
                text: i18nc("@option:radio", "Keyboard shortcut")
                ToolTip.text: i18nc("@info:tooltip", "The command will be triggered with a shortcut. The shortcut can be set after saving.")
                ToolTip.visible: hovered
                ToolTip.delay: 700
            }

            RadioButton {
                property string optionName: "startup"

                checked: optionName === root.type
                text: i18nc("@option:radio", "Run at startup")
                ToolTip.text: i18nc("@info:tooltip", "The command will be run at application startup.")
                ToolTip.visible: hovered
                ToolTip.delay: 700
            }
        }
    }

    footer: ToolBar {
        RowLayout {
            anchors.fill: parent

            ToolButton {
                text: i18nc("@action:intoolbar", "Delete")
                icon.name: "delete"
                visible: root.mode === EditCustomCommand.Mode.Edit
                onClicked: {
                    root.m_customCommandsModel.deleteCustomCommand(root.commandId, root.index)
                    applicationWindow().pageStack.replace(`${root.settingsPath}/CustomCommandsSettings.qml`,
                                                          {m_customCommandsModel: root.m_customCommandsModel})
                }
            }

            Item {

                Layout.fillWidth: true
            }

            ToolButton {
                text: i18nc("@action:intoolbar", "Cancel")
                icon.name: "dialog-cancel"
                onClicked: applicationWindow().pageStack.replace(`${root.settingsPath}/CustomCommandsSettings.qml`,
                                                                 {m_customCommandsModel: root.m_customCommandsModel})
                Layout.alignment: Qt.AlignRight
            }

            ToolButton {
                text: i18nc("@action:intoolbar", "&Save")
                icon.name: "document-save"
                enabled: commandTextField.text !== ""
                onClicked: saveCustomCommand()

                Layout.alignment: Qt.AlignRight

                function saveCustomCommand() : void {
                    if (commandTextField.text === "") {
                        return
                    }
                    if (typeGroup.checkedButton.optionName === "startup") {
                        // execute the user command
                        mpv.userCommand(commandTextField.text)
                    }
                    switch (root.mode) {
                    case EditCustomCommand.Mode.Create:
                        // save new command to config file
                        root.m_customCommandsModel.saveCustomCommand(commandTextField.text,
                                                              osdMessageTextField.text,
                                                              typeGroup.checkedButton.optionName)
                        break
                    case EditCustomCommand.Mode.Edit:
                        // update existing command
                        root.m_customCommandsModel.editCustomCommand(root.index,
                                                              commandTextField.text,
                                                              osdMessageTextField.text,
                                                              typeGroup.checkedButton.optionName)
                        break
                    }
                    applicationWindow().pageStack.replace(`${root.settingsPath}/CustomCommandsSettings.qml`,
                                                          {m_customCommandsModel: root.m_customCommandsModel})
                }

            }
        }
    }
}
