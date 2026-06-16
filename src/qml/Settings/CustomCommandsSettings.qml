/*
 * SPDX-FileCopyrightText: 2021 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import org.kde.ki18n
import org.kde.kirigami as Kirigami

import org.kde.kquickcontrols
import org.kde.haruna
import org.kde.haruna.settings

SettingsBasePage {
    id: root

    required property MpvVideo m_mpv
    required property Kirigami.ApplicationWindow m_settingsWindow

    property string settingsPath: "qrc:/qt/qml/org/kde/haruna/qml/Settings"

    function loadEditCustomCommandPage() : void {
        const props = {
            m_mpv: root.m_mpv,
            m_settingsWindow: root.m_settingsWindow,
        }
        root.m_settingsWindow.pageStack.replace(`${root.settingsPath}/EditCustomCommand.qml`, props)
    }

    ListView {
        id: customCommandsView

        reuseItems: true
        model: Models.customCommandsModel
        delegate: customCommandDelegate

        displaced: Transition {
            NumberAnimation {
                properties: "y"
                duration: Kirigami.Units.shortDuration
            }
        }

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            visible: customCommandsView.count === 0
            text: KI18n.i18nc("@label:textbox", "No custom commands yet")
            helpfulAction: Action {
                text: KI18n.i18nc("@action:button", "&Add Command")
                onTriggered: root.loadEditCustomCommandPage()
            }
        }
    }

    Component {
        id: customCommandDelegate

        Loader {
            id: delegate

            required property int index
            required property string command
            required property string shortcut
            required property string commandId
            required property string osdMessage
            required property string type
            required property bool setOnStartup

            width: customCommandsView.width
            asynchronous: true
            sourceComponent: ItemDelegate {
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
                            const modelIndex = Models.customCommandsModel.index(sourceRow, 0).parent
                            Models.customCommandsModel.moveRows(modelIndex, sourceRow, 1, modelIndex, destinationRow)
                        }
                    }

                    CheckBox {
                        id: toggleCommandCheckBox

                        visible: delegate.type === "startup"
                        checked: delegate.setOnStartup
                        onClicked: {
                            Models.customCommandsModel.toggleCustomCommand(delegate.commandId, delegate.index, checked)
                        }

                        ToolTip {
                            text: KI18n.i18nc("@info:tooltip", "Checked: property will be set at startup\n" +
                                        "Unchecked: property will not be set at startup")
                            visible: toggleCommandCheckBox.hovered && GeneralSettings.showExplanatoryToolTips
                        }
                    }

                    Kirigami.Icon {
                        source: delegate.type === "shortcut" ? "configure-shortcuts" : "code-context"
                        enabled: delegate.setOnStartup
                        Layout.preferredWidth: Kirigami.Units.iconSizes.small
                        Layout.preferredHeight: Kirigami.Units.iconSizes.small
                    }

                    LabelWithTooltip {
                        text: delegate.command
                        elide: Text.ElideRight
                        enabled: delegate.setOnStartup
                        opacity: enabled ? 1.0 : 0.7

                        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                        Layout.fillWidth: true
                        Layout.margins: Kirigami.Units.largeSpacing
                    }

                    Loader {
                        active: delegate.type === "shortcut"
                        asynchronous: true
                        sourceComponent: KeySequenceItem {
                            checkForConflictsAgainst: ShortcutType.None
                            modifierlessAllowed: true
                            keySequence: delegate.shortcut

                            onKeySequenceModified: {
                                if (keySequence.toString() === "" ) {
                                    return
                                }
                                if (keySequence !== delegate.shortcut) {
                                    // use action name (commandId) since this changes the actionsModel
                                    if (!Models.actionsModel.saveShortcut(delegate.commandId, keySequence)) {
                                        keySequence = delegate.shortcut
                                    }
                                }
                            }
                        }
                        Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    }



                    Item { Layout.preferredWidth: Kirigami.Units.smallSpacing }

                    Button {
                        icon.name: "edit-entry"
                        Layout.rightMargin: Kirigami.Units.largeSpacing
                        onClicked: {
                            const properties = {
                                m_mpv: root.m_mpv,
                                m_settingsWindow: root.m_settingsWindow,
                                command: delegate.command,
                                osdMessage: delegate.osdMessage,
                                type: delegate.type,
                                commandId: delegate.commandId,
                                index: delegate.index,
                                mode: EditCustomCommand.Mode.Edit
                            }
                            root.m_settingsWindow.pageStack.replace(`${root.settingsPath}/EditCustomCommand.qml`, properties)
                        }
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

                text: KI18n.i18nc("@action:intoolbar", "&Add")
                icon.name: "list-add"
                onClicked: root.loadEditCustomCommandPage()
                Layout.alignment: Qt.AlignRight
            }
        }
    }

}
