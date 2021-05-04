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

    ListView {

        model: customPropsModel
        delegate: Kirigami.BasicListItem {
            text: model.command
            subtitle: model.osdMessage
        }
    }

    footer: ToolBar {
        RowLayout {
            anchors.fill: parent

            ToolButton {
                text: qsTr("&Add")
                icon.name: "list-add"
                onClicked: applicationWindow().pageStack.replace("qrc:/AddNewCustomProperty.qml")
                Layout.alignment: Qt.AlignRight
            }
        }
    }

}
