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
import AppSettings 1.0

Item {
    id: root

    property bool hasHelp: false
    property string helpFile: ""

    visible: false

    ColumnLayout {
        id: content

        width: parent.width
        spacing: 20

        ListView {
            id: buttonsView

            property int index: -1

            implicitHeight: 50 * (buttonsView.count + 1)
            model: ["Left", "Left.x2", "Middle", "Middle.x2", "Right", "Right.x2", "ScrollUp", "ScrollDown"]
            header: RowLayout {
                Kirigami.ListSectionHeader {
                    text: qsTr("Button")
                    Layout.leftMargin: 5
                    Layout.preferredWidth: 100
                }

                Kirigami.ListSectionHeader {
                    text: qsTr("Action")
                    Layout.leftMargin: 5
                    Layout.fillWidth: true
                }
            }
            delegate: ItemDelegate {
                property string actionLabel: AppSettings.get("Mouse", modelData)
                property string buttonLabel: modelData

                width: content.width
                height: 50

                onDoubleClicked: openSelectActionPopup()

                contentItem: RowLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    Label {
                        text: buttonLabel
                        padding: 10
                        Layout.preferredWidth: 100
                        Layout.fillHeight: true
                    }

                    Label {
                        text: actionLabel
                        Layout.fillWidth: true
                    }

                    Button {
                        flat: true
                        icon.name: "configure"
                        Layout.alignment: Qt.AlignRight
                        onClicked: openSelectActionPopup()
                    }
                }

                function openSelectActionPopup() {
                    buttonsView.index = model.index
                    selectActionPopup.headerTitle = buttonLabel
                    selectActionPopup.open()
                }
            }
            Connections {
                target: selectActionPopup
                function onActionSelected() {
                    const item  = buttonsView.itemAtIndex(buttonsView.index)
                    item.actionLabel = actionName
                    AppSettings.set("Mouse", item.buttonLabel, actionName)
                }
            }
        }

        Label {
            text: qsTr("Double click to edit actions")
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignTop
        }

        SelectActionPopup { id: selectActionPopup }
    }
}
