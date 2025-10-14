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

                required property mouseAction mouseAction
                required property int index
                required property string actionName
                required property string button
                required property string modifier
                required property bool isDoubleClick
                property string dc: delegate.isDoubleClick ? " x2" : ""

                width: ListView.view.width
                highlighted: false

                onClicked: {
                    addActionComponent.createObject(root, {action: mouseAction, isEditMode: true})
                }

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
            id: mouseActionDialog

            property bool canBeSaved: !noActionSelectedMessage.visible && !actionExistsMessage.visible
            property bool isEditMode: false
            property mouseAction action

            title: isEditMode
                   ? i18nc("@title:window", "Edit mouse button action")
                   : i18nc("@title:window", "Add mouse button action")
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
                    text: i18nc("@label:listbox", "Mouse button")
                }

                ComboBox {
                    id: mouseButtonComboBox

                    enabled: !mouseActionDialog.isEditMode
                    textRole: "text"
                    valueRole: "value"
                    model: ListModel {
                        id: mouseButtonsModel
                    }
                    onActivated: {
                        Q_EMIT: root.newMouseActionChanged()
                    }
                    Component.onCompleted: {
                        const left = {
                            text: i18nc("@item:listbox left mouse button", "Left"),
                            value: "Left"
                        }
                        mouseButtonsModel.append(left)

                        const middle = {
                            text: i18nc("@item:listbox middle mouse button", "Middle"),
                            value: "Middle"
                        }
                        mouseButtonsModel.append(middle)

                        const right = {
                            text: i18nc("@item:listbox right mouse button", "Right"),
                            value: "Right"
                        }
                        mouseButtonsModel.append(right)

                        const forward = {
                            text: i18nc("@item:listbox forward mouse button", "Forward"),
                            value: "Forward"
                        }
                        mouseButtonsModel.append(forward)

                        const back = {
                            text: i18nc("@item:listbox back mouse button", "Back"),
                            value: "Back"
                        }
                        mouseButtonsModel.append(back)

                        const scrollUp = {
                            text: i18nc("@item:listbox mouse scroll up", "Scroll up"),
                            value: "ScrollUp"
                        }
                        mouseButtonsModel.append(scrollUp)

                        const scrollDown = {
                            text: i18nc("@item:listbox mouse scroll down", "Scroll down"),
                            value: "ScrollDown"
                        }
                        mouseButtonsModel.append(scrollDown)

                        switch(mouseActionDialog.action.mouseButton) {
                            case MouseActionsModel.Left: {
                                currentIndex = indexOfValue("Left")
                                break
                            }
                            case MouseActionsModel.Middle: {
                                currentIndex = indexOfValue("Middle")
                                break
                            }
                            case MouseActionsModel.Right: {
                                currentIndex = indexOfValue("Right")
                                break
                            }
                            case MouseActionsModel.Forward: {
                                currentIndex = indexOfValue("Forward")
                                break
                            }
                            case MouseActionsModel.Back: {
                                currentIndex = indexOfValue("Back")
                                break
                            }
                            case MouseActionsModel.ScrollUp: {
                                currentIndex = indexOfValue("ScrollUp")
                                break
                            }
                            case MouseActionsModel.ScrollDown: {
                                currentIndex = indexOfValue("ScrollDown")
                                break
                            }
                        }
                    }
                    Layout.bottomMargin: Kirigami.Units.largeSpacing
                }

                Label {
                    text: i18nc("@label:listbox", "Modifier key")
                }

                ComboBox {
                    id: modifierComboBox

                    enabled: !mouseActionDialog.isEditMode
                    textRole: "text"
                    valueRole: "value"
                    model: ListModel {
                        id: modifiersModel
                    }
                    onActivated: {
                        Q_EMIT: root.newMouseActionChanged()
                    }

                    Component.onCompleted: {
                        const noModifier = {
                            text: i18nc("@item:listbox no modifier key", "No modifier"),
                            value: "NoModifier"
                        }
                        modifiersModel.append(noModifier)

                        const control = {
                            text: i18nc("@item:listbox control modifier key", "Control"),
                            value: "Control"
                        }
                        modifiersModel.append(control)

                        const shift = {
                            text: i18nc("@item:listbox shift modifier key", "Shift"),
                            value: "Shift"
                        }
                        modifiersModel.append(shift)

                        const alt = {
                            text: i18nc("@item:listbox alt modifier key", "Alt"),
                            value: "Alt"
                        }
                        modifiersModel.append(alt)

                        const meta = {
                            text: i18nc("@item:listbox meta modifier key", "Meta"),
                            value: "Meta"
                        }
                        modifiersModel.append(meta)

                        switch(mouseActionDialog.action.modifier) {
                            case Qt.NoModifier: {
                                currentIndex = indexOfValue("NoModifier")
                                break
                            }
                            case Qt.ControlModifier: {
                                currentIndex = indexOfValue("Control")
                                break
                            }
                            case Qt.ShiftModifier: {
                                currentIndex = indexOfValue("Shift")
                                break
                            }
                            case Qt.AltModifier: {
                                currentIndex = indexOfValue("Alt")
                                break
                            }
                            case Qt.MetaModifier: {
                                currentIndex = indexOfValue("Meta")
                                break
                            }
                        }
                    }
                    Layout.bottomMargin: Kirigami.Units.largeSpacing
                }

                CheckBox {
                    id: isDoubleClickCheckBox

                    text: i18nc("@label:check whether mouse button action should trigger on double click",
                                "Trigger on double click")
                    enabled: !mouseActionDialog.isEditMode && mouseButtonComboBox.currentIndex < 5
                    checked: mouseActionDialog.action.isDoubleClick
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
                            selectActionPopup.open()
                        }
                    }

                    Label {
                        id: selectedActionLabel

                        text: mouseActionDialog.action.actionName
                        font.weight: Font.Bold
                        Layout.fillWidth: true
                    }

                    Button {
                        text: i18nc("@action:button", "Change action")
                        visible: selectedActionLabel.text
                        onClicked: {
                            selectActionPopup.open()
                        }
                    }
                }

                Kirigami.InlineMessage {
                    id: noActionSelectedMessage

                    text: i18nc("@info", "No action selected")
                    type: Kirigami.MessageType.Warning
                    visible: selectedActionLabel.text === ""

                    Layout.fillWidth: true
                    Layout.preferredHeight: 50
                    Layout.topMargin: Kirigami.Units.largeSpacing
                }

                Kirigami.InlineMessage {
                    id: actionExistsMessage

                    text: {
                        const model = root.m_mouseActionsModel
                        const button = mouseButtonComboBox.currentValue
                        const modifier = modifierComboBox.currentValue
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

                onActionSelected: function(actionName) {
                    mouseActionDialog.action.actionName = actionName
                    selectedActionLabel.text = HarunaApp.actions[actionName].text
                    Q_EMIT: root.newMouseActionChanged()
                }
            }

            Connections {
                target: root
                function onNewMouseActionChanged() {
                    const button = mouseButtonComboBox.currentValue
                    const modifier = modifierComboBox.currentValue
                    const isDoubleClick = isDoubleClickCheckBox.checked

                    if (mouseActionDialog.isEditMode) {
                        return
                    }

                    if (root.m_mouseActionsModel.actionExists(button, modifier, isDoubleClick)) {
                        actionExistsMessage.visible = true
                    } else {
                        actionExistsMessage.visible = false
                    }
                    let okBtn = mouseActionDialog.standardButton(Dialog.Ok)
                    okBtn.enabled = mouseActionDialog.canBeSaved
                }
            }

            onAccepted: {
                if (mouseActionDialog.isEditMode) {
                    root.m_mouseActionsModel.editAction(mouseActionDialog.action)
                    return
                }

                root.m_mouseActionsModel.addAction(mouseActionDialog.action.actionName,
                                                   mouseButtonComboBox.currentValue,
                                                   modifierComboBox.currentValue,
                                                   isDoubleClickCheckBox.checked)
            }
        }
    }
}
