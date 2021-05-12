/*
 * SPDX-FileCopyrightText: 2021 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

import org.kde.kirigami 2.11 as Kirigami
import com.georgefb.haruna 1.0
import Haruna.Components 1.0

SettingsBasePage {
    id: root

    hasHelp: false
    helpFile: ""

    Component {
        id: customPropDelegate

        Kirigami.AbstractListItem {
            id: customPropItem
            height: Kirigami.Units.gridUnit * 3
            padding: 0

            contentItem: RowLayout {
                anchors.fill: parent
                spacing: 0

                Kirigami.ListItemDragHandle {
                    listItem: customPropItem
                    listView: customPropsView
                    onMoveRequested: customPropsModel.moveRows(oldIndex, newIndex)
                }

                LabelWithTooltip {
                    text: model.command
                    toolTipText: text
                    elide: Text.ElideRight

                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    Layout.fillWidth: true
                    Layout.margins: Kirigami.Units.largeSpacing
                }

                Button {
                    text: i18n("Shortcut")
                    icon.name: "configure-shortcuts"

                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    Layout.margins: Kirigami.Units.largeSpacing
                }
            }
        }
    }

    ListView {
        id: customPropsView

        model: customPropsModel
        delegate: Kirigami.DelegateRecycler {
            width: customPropsView.width
            sourceComponent: customPropDelegate
        }
    }

    footer: ToolBar {
        RowLayout {
            anchors.fill: parent

            ToolButton {
                id: addButton

                text: i18n("&Add")
                icon.name: "list-add"
                checkable: true
                checked: false
                onClicked: addMenu.toggleMenu()
                Layout.alignment: Qt.AlignRight

                Menu {
                    id: addMenu

                    property bool isOpen: false

                    visible: isOpen
                    x: 0
                    y: -1

                    MenuItem {
                        text: i18n("Add start up property")
                        onTriggered: applicationWindow().pageStack.replace("qrc:/AddNewCustomProperty.qml",
                                                                           {isAction: false})
                    }

                    MenuItem {
                        text: i18n("Add action")
                        onTriggered: applicationWindow().pageStack.replace("qrc:/AddNewCustomProperty.qml",
                                                                           {isAction: true})
                    }

                    function toggleMenu() {
                        if (!addButton.checked) {
                            return
                        }

                        addMenu.visible = !addMenu.visible
                        const menuHeight = addMenu.count * addMenu.itemAt(0).implicitHeight
                        addMenu.popup(addButton, 0, -menuHeight)
                    }
                }
            }
        }
    }

}
