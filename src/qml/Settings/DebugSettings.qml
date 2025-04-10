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
            text: i18nc("@label:textbox", "Config folder")
        }

        RowLayout {
            Kirigami.ActionTextField {
                id: configFolderField

                text: HarunaApp.configFolderPath()
                readOnly: true
                rightActions: Kirigami.Action {
                    icon.name: "document-open-folder"
                    visible: configFolderField.text !== ""
                    text: i18nc("@info:tooltip", "Open config folder")
                    onTriggered: {
                        Qt.openUrlExternally(HarunaApp.configFolderPath())
                    }
                }
                Layout.fillWidth: true
            }
        }

        Item { Layout.preferredHeight: Kirigami.Units.largeSpacing }

        Label {
            text: i18nc("@label:textbox", "Config file")
        }

        RowLayout {
            Kirigami.ActionTextField {
                id: configFileField

                text: HarunaApp.configFilePath()
                readOnly: true
                rightActions: Kirigami.Action {
                    icon.name: "document-open"
                    visible: configFileField.text !== ""
                    text: i18nc("@info:tooltip", "Open config file")
                    onTriggered: {
                        Qt.openUrlExternally(HarunaApp.configFilePath())
                    }
                }
                Layout.fillWidth: true
            }
        }

        Item { Layout.preferredHeight: Kirigami.Units.largeSpacing }

        Label {
            text: i18nc("@label:textbox", "Custom commands config file")
        }

        RowLayout {
            Kirigami.ActionTextField {
                id: ccConfigFileField

                text: HarunaApp.ccConfigFilePath()
                readOnly: true
                rightActions: Kirigami.Action {
                    icon.name: "document-open"
                    visible: ccConfigFileField.text !== ""
                    text: i18nc("@info:tooltip", "Open custom commands config file")
                    onTriggered: {
                        Qt.openUrlExternally(HarunaApp.ccConfigFilePath())
                    }
                }
                Layout.fillWidth: true
            }
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
}
