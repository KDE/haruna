/*
 * SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQml
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import org.kde.kirigami as Kirigami
import org.kde.haruna
import org.kde.haruna.settings

SettingsBasePage {
    id: root

    required property MpvVideo m_mpv

    ColumnLayout {
        id: content

        Label {
            text: i18nc("@label:textbox", "Config file")
        }

        ConfigFileField {
            configFile: Global.ConfigFile.Main

            Layout.fillWidth: true
        }

        Item { Layout.preferredHeight: Kirigami.Units.largeSpacing }

        Label {
            text: i18nc("@label:textbox", "Custom commands config file")
        }

        ConfigFileField {
            configFile: Global.ConfigFile.CustomCommands

            Layout.fillWidth: true
        }

        Item { Layout.preferredHeight: Kirigami.Units.largeSpacing }

        Label {
            text: i18nc("@label:textbox", "Shortcuts config file")
        }

        ConfigFileField {
            configFile: Global.ConfigFile.Shortcuts

            Layout.fillWidth: true
        }

        Item { Layout.preferredHeight: Kirigami.Units.largeSpacing }

        Label {
            text: i18nc("@label:textbox", "Playlists config file")
        }

        ConfigFileField {
            configFile: Global.ConfigFile.PlaylistCache

            Layout.fillWidth: true
        }

        Item { Layout.preferredHeight: Kirigami.Units.largeSpacing }

        Label {
            text: i18nc("@info:usagetip", "If fields are empty that means there is no config file/folder (e.g when using the default settings).")
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            Layout.fillWidth: true
        }

        Item { Layout.preferredHeight: Kirigami.Units.largeSpacing }

        CheckBox {
            text: i18nc("@option:check", "Enable mpv logging")
            checked: InformationSettings.mpvLogging
            visible: false
            onClicked: {
                root.m_mpv.setProperty(MpvProperties.Terminal, checked)
                InformationSettings.mpvLogging = checked
                InformationSettings.save()
            }
        }
    }


    component ConfigFileField: Kirigami.ActionTextField {
        id: configFileField

        property int configFile: Global.ConfigFile.Main

        text: Global.configFilePath(configFile)
        readOnly: true
        rightActions: [
            Kirigami.Action {
                icon.name: "document-open"
                visible: configFileField.text !== ""
                text: i18nc("@info:tooltip", "Open file")
                onTriggered: {
                    const url = HarunaApp.pathToUrl(Global.configFilePath(configFileField.configFile))
                    Qt.openUrlExternally(url)
                }
            },
            Kirigami.Action {
                icon.name: "document-open-folder"
                visible: configFileField.text !== ""
                text: i18nc("@info:tooltip", "Open parent folder")
                onTriggered: {
                    const path = Global.configFilePath(configFileField.configFile)
                    Global.highlightInFileManager(path)
                }
            }
        ]
    }
}
