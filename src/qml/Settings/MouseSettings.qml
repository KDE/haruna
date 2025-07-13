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

    required property MouseActionsModel m_mouseActionsModel

    signal newMouseActionChanged()

    ColumnLayout {
        id: content

        anchors.fill: parent
        spacing: Kirigami.Units.largeSpacing

        ListView {
            id: mouseActionsListView

            model: root.m_mouseActionsModel
            delegate: ItemDelegate {
                id: delegate

                required property int index
                required property string actionName
                required property string button
                required property string modifier
                required property bool isDoubleClick
                property string dc: delegate.isDoubleClick ? " x2" : ""

                width: ListView.view.width
                highlighted: false

                contentItem: RowLayout {
                    Kirigami.Chip {
                        text: delegate.modifier
                        visible: text
                        closable: false
                        checkable: false
                    }

                    Label {
                        text: "+"
                        visible: delegate.modifier
                    }

                    Kirigami.Chip {
                        text: delegate.button + delegate.dc
                        closable: false
                        checkable: false
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    Label {
                        text: HarunaApp.actions[delegate.actionName].text
                    }

                    ToolButton {
                        visible: delegate.actionName
                        icon.name: "delete"
                        onClicked: {
                            root.m_mouseActionsModel.removeAction(delegate.index)
                        }

                        ToolTip {
                            text: i18nc("@info:tooltip", "Remove mouse button action")
                        }
                    }
                }
            }

            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        Item {
            Layout.preferredWidth: Kirigami.Units.gridUnit
            Layout.preferredHeight: Kirigami.Units.gridUnit
        }
    }

    footer: ToolBar {
        RowLayout {
            anchors.fill: parent

            ToolButton {
                id: addButton

                text: i18nc("@action:intoolbar add a mouse button action", "&Add")
                icon.name: "list-add"
                onClicked: {
                    addActionComponent.createObject()
                }

                Layout.alignment: Qt.AlignRight
            }
        }
    }

    Component {
        id: addActionComponent

        Dialog {
            id: newMouseActionDialog

            property bool canBeSaved: !noActionSelectedMessage.visible && !actionExistsMessage.visible

            title: i18nc("@title:window", "Add mouse button action")
            parent: content
            width: Math.min(parent.width, 400)
            height: Math.min(parent.height, 600)
            visible: true
            standardButtons: Dialog.Ok | Dialog.Cancel
            closePolicy: Popup.NoAutoClose | Popup.CloseOnEscape
            modal: true

            Component.onCompleted: {
                Q_EMIT: root.newMouseActionChanged()
            }

            ColumnLayout {
                width: parent.width

                Label {
                    text: i18nc("@label", "Mouse button")
                }

                ComboBox {
                    id: mouseButtonComboBox
                    property string value: modelValues[currentIndex]
                    property list<string> modelValues: [
                        "Left", "Middle", "Right",
                        "Forward", "Back",
                        "ScrollUp", "ScrollDown"
                    ]
                    model: [
                        i18nc("@label:listbox left mouse button", "Left"),
                        i18nc("@label:listbox middle mouse button", "Middle"),
                        i18nc("@label:listbox right mouse button", "Right"),
                        i18nc("@label:listbox forward mouse button", "Forward"),
                        i18nc("@label:listbox back mouse button", "Back"),
                        i18nc("@label:listbox mouse scroll up", "Scroll up"),
                        i18nc("@label:listbox mouse scroll down", "Scroll down")
                    ]
                    onActivated: {
                        Q_EMIT: root.newMouseActionChanged()
                    }
                    Layout.bottomMargin: Kirigami.Units.largeSpacing
                }

                Label {
                    text: i18nc("@label", "Modifier key")
                }

                ComboBox {
                    id: modifierComboBox
                    property string value: modelValues[currentIndex]
                    property list<string> modelValues: ["NoModifier", "Control", "Shift", "Alt", "Meta"]
                    model: [
                        i18nc("@label:listbox no modifier key", "No modifier"),
                        i18nc("@label:listbox control modifier key", "Control"),
                        i18nc("@label:listbox shift modifier key", "Shift"),
                        i18nc("@label:listbox alt modifier key", "Alt"),
                        i18nc("@label:listbox meta modifier key", "Meta")
                    ]
                    onActivated: {
                        Q_EMIT: root.newMouseActionChanged()
                    }
                    Layout.bottomMargin: Kirigami.Units.largeSpacing
                }

                CheckBox {
                    id: isDoubleClickCheckBox
                    text: i18nc("@label:check whether mouse button action shoud trigger on double click",
                                "Trigger on double click")
                    enabled: mouseButtonComboBox.currentIndex < 5
                    onClicked: {
                        Q_EMIT: root.newMouseActionChanged()
                    }
                    Layout.bottomMargin: Kirigami.Units.largeSpacing
                }

                RowLayout {
                    Label {
                        text: i18nc("@label", "Action")
                    }
                }

                RowLayout {
                    Layout.fillWidth: true

                    Button {
                        text: i18nc("@action:button select mouse button action", "Select action")
                        visible: !selectedActionLabel.text
                        onClicked: {
                            selectActionPopup.title = i18nc("@title:window select mouse button action",
                                                            "Select action")
                            selectActionPopup.open()
                        }
                    }

                    Label {
                        id: selectedActionLabel
                        property string actionName
                        font.weight: Font.Bold
                        Layout.fillWidth: true
                    }

                    Button {
                        text: "Change action"
                        visible: selectedActionLabel.text
                        onClicked: {
                            selectActionPopup.title = i18nc("@action:button change mouse button action ",
                                                            "Change action")
                            selectActionPopup.open()
                        }
                    }
                }

                Kirigami.InlineMessage {
                    id: noActionSelectedMessage

                    text: i18nc("@info", "No action selected")
                    type: Kirigami.MessageType.Warning
                    visible: selectedActionLabel.actionName === ""

                    Layout.fillWidth: true
                    Layout.preferredHeight: 50
                    Layout.topMargin: Kirigami.Units.largeSpacing
                }

                Kirigami.InlineMessage {
                    id: actionExistsMessage

                    text: {
                        const model = root.m_mouseActionsModel
                        const button = mouseButtonComboBox.value
                        const modifier = modifierComboBox.value
                        const isDoubleClick = isDoubleClickCheckBox.checked
                        const actionName = model.getAction(model.stringToMouseButton(button),
                                                           model.stringToModifier(modifier),
                                                           isDoubleClickCheckBox.checked)
                        if (actionName === "") {
                            return ""
                        }
                        return i18nc("@info", "Mouse button combination is already assigned to “%1”",
                                     HarunaApp.actions[actionName].text)
                    }

                    type: Kirigami.MessageType.Warning

                    Layout.fillWidth: true
                    Layout.preferredHeight: 50
                    Layout.topMargin: Kirigami.Units.largeSpacing
                }

            }

            SelectActionPopup {
                id: selectActionPopup

                width: Math.min(parent.width, 400)
                height: Math.min(parent.height, 600)
                onActionSelected: function(actionName) {
                    selectedActionLabel.text = HarunaApp.actions[actionName].text
                    selectedActionLabel.actionName = actionName
                    Q_EMIT: root.newMouseActionChanged()
                }
            }

            Connections {
                target: root
                function onNewMouseActionChanged() {
                    const button = mouseButtonComboBox.value
                    const modifier = modifierComboBox.value
                    const isDoubleClick = isDoubleClickCheckBox.checked

                    if (root.m_mouseActionsModel.actionExists(button, modifier, isDoubleClick)) {
                        actionExistsMessage.visible = true
                    } else {
                        actionExistsMessage.visible = false
                    }
                    let okBtn = newMouseActionDialog.standardButton(Dialog.Ok)
                    okBtn.enabled = newMouseActionDialog.canBeSaved
                }
            }

            onAccepted: {
                root.m_mouseActionsModel.addAction(selectedActionLabel.actionName,
                                                   mouseButtonComboBox.value,
                                                   modifierComboBox.value,
                                                   isDoubleClickCheckBox.checked)
            }
        }
    }
}
