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

    property bool isAction: true
    property int id: customPropsModel.rowCount() + 1

    GridLayout {
        columns: 2

        Label {
            text: i18n("Command")
            Layout.alignment: Qt.AlignRight
        }

        TextField {
            id: commandTextField

            placeholderText: "command"
            Layout.fillWidth: true
        }

        Label {
            text: i18n("OSD message")
            visible: root.isAction
            Layout.alignment: Qt.AlignRight
        }

        TextField {
            id: osdMsgTextField

            visible: root.isAction
            placeholderText: "osd message"
            Layout.fillWidth: true
        }
    }

    footer: ToolBar {
        RowLayout {
            anchors.fill: parent

            ToolButton {
                text: i18n("&Save")
                icon.name: "document-save"
                enabled: commandTextField.text !== ""
                onClicked: {
                    if (commandTextField.text !== "") {
                        customPropsModel.saveCustomProperty("Command_" + root.id,
                                                            commandTextField.text,
                                                            osdMsgTextField.text,
                                                            root.isAction)
                        customPropsModel.getProperties()
                    }
                    applicationWindow().pageStack.replace("qrc:/CustomProperties.qml")
                }

                Layout.alignment: Qt.AlignRight
            }
        }
    }
}
