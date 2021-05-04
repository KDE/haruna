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

    GridLayout {
        columns: 2

        Label {
            text: qsTr("Command")
            Layout.alignment: Qt.AlignRight
        }

        TextField {
            placeholderText: "command"
            Layout.fillWidth: true
        }

        Label {
            text: qsTr("OSD message")
            Layout.alignment: Qt.AlignRight
        }

        TextField {
            placeholderText: "osd message"
            Layout.fillWidth: true
        }

        CheckBox {
            id: setAtStartUp

            text: qsTr("Set at start up")
            Layout.columnSpan: 2
            Layout.row: 2
            Layout.column: 1
        }

        Button {
            text: qsTr("Set shortcut")
            enabled: !setAtStartUp.checked
            Layout.columnSpan: 2
            Layout.row: 3
            Layout.column: 1
        }
    }

    footer: ToolBar {
        RowLayout {
            anchors.fill: parent

            ToolButton {
                text: qsTr("&Save")
                icon.name: "document-save"
                onClicked: applicationWindow().pageStack.replace("qrc:/CustomProperties.qml")
                Layout.alignment: Qt.AlignRight
            }
        }
    }
}
