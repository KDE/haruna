/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import org.kde.kirigami as Kirigami
import org.kde.haruna
import org.kde.haruna.mpvproperties
import org.kde.haruna.settings
import Haruna.Components

SettingsBasePage {
    id: root

    hasHelp: false
    helpFile: ""

    GridLayout {
        id: content

        columns: 2

        Label {
            text: i18nc("@label:textbox", "Preferred language")
            Layout.alignment: Qt.AlignRight
        }

        TextField {
            id: preferredLanguage

            text: AudioSettings.preferredLanguage
            placeholderText: i18nc("placeholder text", "eng, ger etc.")
            Layout.fillWidth: true
            onEditingFinished: save()

            Connections {
                target: root
                function onSave() {
                    preferredLanguage.save()
                }
            }

            function save() {
                AudioSettings.preferredLanguage = text
                AudioSettings.save()
                mpv.setProperty(MpvProperties.AudioLanguage, text.replace(/\s+/g, ''))
            }
        }

        Label {
            text: i18nc("@label:spinbox", "Preferred track")
            Layout.alignment: Qt.AlignRight
        }

        SpinBox {
            from: 0
            to: 100
            value: AudioSettings.preferredTrack
            editable: true
            onValueChanged: {
                AudioSettings.preferredTrack = value
                AudioSettings.save()
                if (value === 0) {
                    mpv.audioId = "auto"

                } else {
                    mpv.audioId = value
                }
            }
        }

        // Volume Step
        Label {
            text: i18nc("@label:spinbox", "Volume step")
            Layout.alignment: Qt.AlignRight
        }

        SpinBox {
            id: volumeStep
            editable: true
            from: 0
            to: 100
            value: GeneralSettings.volumeStep
            onValueChanged: {
                if (root.visible) {
                    GeneralSettings.volumeStep = volumeStep.value
                    GeneralSettings.save()
                }
            }
        }

        Item {
            width: Kirigami.Units.gridUnit
            height: Kirigami.Units.gridUnit
        }
    }
}
