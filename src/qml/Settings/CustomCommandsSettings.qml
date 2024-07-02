/*
 * SPDX-FileCopyrightText: 2021 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import org.kde.kirigami as Kirigami
import org.kde.kquickcontrols
import org.kde.haruna

SettingsBasePage {
    id: root

    property string settingsPath: "qrc:/qt/qml/org/kde/haruna/qml/Settings"

    hasHelp: true
    helpFile: ":/CustomCommandsSettings.html"

    ListView {
        id: customCommandsView

        reuseItems: true
        model: customCommandsModel
        delegate: Loader {
            required property var model
            required property int index
            width: customCommandsView.width
            sourceComponent: customCommandDelegate
        }


        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            visible: customCommandsView.count === 0
            text: i18nc("@label:textbox", "No custom commands yet")
            helpfulAction: Action {
                text: i18nc("@action:button", "&Add command")
                onTriggered: applicationWindow().pageStack.replace(`${root.settingsPath}/EditCustomCommand.qml`)
            }
        }
    }

    Component {
        id: customCommandDelegate

        ItemDelegate {
            id: customCommandItem

            height: Kirigami.Units.gridUnit * 3
            padding: 0

            contentItem: RowLayout {
                anchors.fill: parent
                spacing: 0

                Kirigami.ListItemDragHandle {
                    listItem: customCommandItem
                    listView: customCommandsView
                    onMoveRequested: function (sourceRow, destinationRow) {
                        const modelIndex = customCommandsModel.index(sourceRow, 0).parent
                        customCommandsModel.moveRows(modelIndex, sourceRow, 1, modelIndex, destinationRow)
                    }
                }

                CheckBox {
                    visible: model.type === "startup"
                    checked: model.setOnStartup
                    onCheckStateChanged: {
                        customCommandsModel.toggleCustomCommand(model.commandId, model.index, checked)
                    }

                    ToolTip {
                        text: i18nc("@info:tooltip", "Checked: property will be set at startup\n" +
                                    "Unchecked: property will not be set at startup")
                    }
                }

                Kirigami.Icon {
                    source: model.type === "shortcut" ? "configure-shortcuts" : "code-context"
                    width: Kirigami.Units.iconSizes.small
                    height: Kirigami.Units.iconSizes.small
                    enabled: model.setOnStartup
                }

                LabelWithTooltip {
                    text: model.command
                    elide: Text.ElideRight
                    enabled: model.setOnStartup
                    opacity: enabled ? 1.0 : 0.7

                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    Layout.fillWidth: true
                    Layout.margins: Kirigami.Units.largeSpacing
                }

                Loader {
                    active: model.type === "shortcut"
                    sourceComponent: KeySequenceItem {
                        checkForConflictsAgainst: ShortcutType.None
                        modifierlessAllowed: true
                        keySequence: model.shortcut

                        onKeySequenceChanged: {
                            if (keySequence !== model.shortcut) {
                                // use action name (commandId) since this changes the actionsModel
                                if (!actionsModel.saveShortcut(model.commandId, keySequence)) {
                                    keySequence = model.shortcut
                                }
                            }
                        }
                    }
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                }



                Item { width: Kirigami.Units.smallSpacing }

                Button {
                    icon.name: "edit-entry"
                    Layout.rightMargin: Kirigami.Units.largeSpacing
                    onClicked: {
                        const properties = {
                            command: model.command,
                            osdMessage: model.osdMessage,
                            type: model.type,
                            commandId: model.commandId,
                            index: model.index,
                            mode: EditCustomCommand.Mode.Edit
                        }
                        applicationWindow().pageStack.replace(`${root.settingsPath}/EditCustomCommand.qml`, properties)
                    }
                }
            }
        }
    }

    footer: ToolBar {
        visible: customCommandsView.count > 0

        RowLayout {
            anchors.fill: parent

            ToolButton {
                id: addButton

                text: i18nc("@action:intoolbar", "&Add")
                icon.name: "list-add"
                onClicked: applicationWindow().pageStack.replace(`${root.settingsPath}/EditCustomCommand.qml`)
                Layout.alignment: Qt.AlignRight
            }
        }
    }

}
