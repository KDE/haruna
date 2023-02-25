/*
 * SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQml 2.12
import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

import org.kde.kirigami 2.11 as Kirigami
import org.kde.haruna 1.0
import Haruna.Components 1.0

SettingsBasePage {
    id: root

    hasHelp: true
    helpFile: ""

    ColumnLayout {
        id: content

        Label {
            text: i18nc("@label:textbox", "Config folder")
        }

        RowLayout {
            TextField {
                id: configFolderField

                text: app.configFolderPath(false)
                readOnly: true
                Layout.fillWidth: true
            }

            Button {
                icon.name: "folder"
                enabled: configFolderField.text !== ""
                onClicked: {
                    Qt.openUrlExternally(app.configFolderPath())
                }

                ToolTip {
                    text: i18nc("@info:tooltip", "Open config folder")
                }
            }
        }

        Item { height: Kirigami.Units.largeSpacing }

        Label {
            text: i18nc("@label:textbox", "Config file")
        }

        RowLayout {
            TextField {
                id: configFileField

                text: app.configFilePath(false)
                readOnly: true
                Layout.fillWidth: true
            }

            Button {
                icon.name: "text-plain"
                enabled: configFileField.text !== ""
                onClicked: {
                    Qt.openUrlExternally(app.configFilePath())
                }

                ToolTip {
                    text: i18nc("@info:tooltip", "Open config file")
                }
            }
        }

        Item { height: Kirigami.Units.largeSpacing }

        Label {
            text: i18nc("@label:textbox", "Custom commands config file")
        }

        RowLayout {
            TextField {
                id: ccConfigFileField

                text: app.ccConfigFilePath(false)
                readOnly: true
                Layout.fillWidth: true
            }

            Button {
                icon.name: "text-plain"
                enabled: ccConfigFileField.text !== ""
                onClicked: {
                    Qt.openUrlExternally(app.ccConfigFilePath())
                }

                ToolTip {
                    text: i18nc("@info:tooltip", "Open custom commands config file")
                }
            }
        }

        Item { height: Kirigami.Units.largeSpacing }

        Label {
            text: i18nc("@info:usagetip", "If fields are empty that means there is no config file/folder (e.g when using the default settings).")
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            Layout.fillWidth: true
        }

        Item { height: Kirigami.Units.largeSpacing }

        CheckBox {
            text: i18nc("@option:check", "Enable mpv logging")
            checked: InformationSettings.mpvLogging
            onCheckedChanged: {
                mpv.setProperty("terminal", checked)
                InformationSettings.mpvLogging = checked
                InformationSettings.save()
            }
        }
    }
}
