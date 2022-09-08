/*
 * SPDX-FileCopyrightText: 2021 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

import org.kde.kirigami 2.12 as Kirigami
import org.kde.kquickcontrols 2.0
import org.kde.haruna 1.0
import Haruna.Components 1.0

SettingsBasePage {
    id: root

    hasHelp: true
    helpFile: ":/CustomCommandsSettings.html"
    docPage: "help:/haruna/CustomCommandsSettings.html"

    ListView {
        id: customCommandsView

        model: customCommandsModel
        delegate: Kirigami.DelegateRecycler {
            width: customCommandsView.width
            sourceComponent: customCommandDelegate
        }
        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            visible: customCommandsView.count === 0
            text: i18n("No custom commands yet")
            helpfulAction: Action {
                text: i18n("&Add command")
                onTriggered: applicationWindow().pageStack.replace("qrc:/EditCustomCommand.qml")
            }
        }
    }

    Component {
        id: customCommandDelegate

        Kirigami.AbstractListItem {
            id: customCommandItem

            height: Kirigami.Units.gridUnit * 3
            padding: 0

            contentItem: RowLayout {
                anchors.fill: parent
                spacing: 0

                Kirigami.ListItemDragHandle {
                    listItem: customCommandItem
                    listView: customCommandsView
                    onMoveRequested: customCommandsModel.moveRows(oldIndex, newIndex)
                }

                CheckBox {
                    visible: model.type === "startup"
                    checked: model.setOnStartup
                    onCheckStateChanged: {
                        customCommandsModel.toggleCustomCommand(model.commandId, model.index, checked)
                    }

                    ToolTip {
                        text: i18n("Don't set on next startup")
                        delay: 0
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

                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    Layout.fillWidth: true
                    Layout.margins: Kirigami.Units.largeSpacing
                }

                Loader {
                    active: model.type === "shortcut"
                    sourceComponent: KeySequenceItem {
                        property var oldKeySequence
                        property var shortcut: model.shortcut

                        checkForConflictsAgainst: ShortcutType.None
                        modifierlessAllowed: true
                        onShortcutChanged: {
                            keySequence = shortcut
                            oldKeySequence = shortcut
                        }

                        onKeySequenceChanged: {
                            if (keySequence !== model.shortcut) {
                                // use action name (commandId) since this changes the actionsModel
                                if (!actionsModel.saveShortcut(model.commandId, keySequence)) {
                                    keySequence = oldKeySequence
                                }
                            }
                        }

                        Component.onCompleted: {
                            oldKeySequence = model.shortcut
                            keySequence = model.shortcut
                        }

                    }
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                }



                Item { width: Kirigami.Units.smallSpacing }

                Button {
                    icon.name: "edit-entry"
                    enabled: model.setOnStartup
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
                        applicationWindow().pageStack.replace("qrc:/EditCustomCommand.qml", properties)
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

                text: i18n("&Add")
                icon.name: "list-add"
                onClicked: applicationWindow().pageStack.replace("qrc:/EditCustomCommand.qml")
                Layout.alignment: Qt.AlignRight
            }
        }
    }

}
