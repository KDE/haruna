/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQml 2.13
import QtQuick 2.0
import QtQuick.Layouts 1.13
import QtQuick.Controls 2.13
import org.kde.kirigami 2.11 as Kirigami

Item {
    id: root

    property alias contentHeight: content.height

    visible: false
    anchors.fill: parent

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
                property alias actionLabel: actionLabel
                property alias buttonLabel: buttonLabel

                width: content.width
                height: 50

                onDoubleClicked: openSelectActionPopup()

                contentItem: RowLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    Label {
                        id: buttonLabel

                        text: modelData
                        padding: 10
                        Layout.preferredWidth: 100
                        Layout.fillHeight: true
                    }

                    Label {
                        id: actionLabel

                        text: settings.get("Mouse", modelData)
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
                    selectActionPopup.headerTitle = buttonLabel.text
                    selectActionPopup.open()
                }
            }
            Connections {
                target: selectActionPopup
                onActionSelected: {
                    const item  = buttonsView.itemAtIndex(buttonsView.index)
                    item.actionLabel.text = actionName
                    settings.set("Mouse", item.buttonLabel.text, actionName)
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
