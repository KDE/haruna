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
            Kirigami.ActionTextField {
                id: configFolderField

                text: app.urlExists(app.configFolderPath(false)) ? app.configFolderPath(false) : ""
                readOnly: true
                rightActions: Kirigami.Action {
                    icon.name: "document-open-folder"
                    visible: configFolderField.text !== ""
                    text: i18nc("@info:tooltip", "Open config folder")
                    onTriggered: {
                        Qt.openUrlExternally(app.configFolderPath())
                    }
                }
                Layout.fillWidth: true
            }
        }

        Item { height: Kirigami.Units.largeSpacing }

        Label {
            text: i18nc("@label:textbox", "Config file")
        }

        RowLayout {
            Kirigami.ActionTextField {
                id: configFileField

                text: app.urlExists(app.configFilePath(false)) ? app.configFilePath(false) : ""
                readOnly: true
                rightActions: Kirigami.Action {
                    icon.name: "document-open"
                    visible: configFileField.text !== ""
                    text: i18nc("@info:tooltip", "Open config file")
                    onTriggered: {
                        Qt.openUrlExternally(app.configFilePath())
                    }
                }
                Layout.fillWidth: true
            }
        }

        Item { height: Kirigami.Units.largeSpacing }

        Label {
            text: i18nc("@label:textbox", "Custom commands config file")
        }

        RowLayout {
            Kirigami.ActionTextField {
                id: ccConfigFileField

                text: app.urlExists(app.ccConfigFilePath(false)) ? app.ccConfigFilePath(false) : ""
                readOnly: true
                rightActions: Kirigami.Action {
                    icon.name: "document-open"
                    visible: ccConfigFileField.text !== ""
                    text: i18nc("@info:tooltip", "Open custom commands config file")
                    onTriggered: {
                        Qt.openUrlExternally(app.ccConfigFilePath())
                    }
                }
                Layout.fillWidth: true
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
            visible: false
            onCheckedChanged: {
                mpv.setProperty(Mpv.Terminal, checked)
                InformationSettings.mpvLogging = checked
                InformationSettings.save()
            }
        }
    }
}
