/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

import org.kde.kirigami 2.11 as Kirigami
import org.kde.haruna 1.0
import Haruna.Components 1.0

SettingsBasePage {
    id: root

    hasHelp: false
    helpFile: ""

    GridLayout {
        id: content

        columns: 2

        Label {
            text: i18n("Preferred language")
            Layout.alignment: Qt.AlignRight
        }

        TextField {
            text: AudioSettings.preferredLanguage
            placeholderText: i18n("eng,ger etc.")
            Layout.fillWidth: true
            onTextEdited: {
                AudioSettings.preferredLanguage = text
                AudioSettings.save()
                mpv.setProperty("alang", text)
            }

            ToolTip {
                text: i18n("Do not use spaces.")
            }
        }

        Label {
            text: i18n("Preferred track")
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
                    mpv.setProperty("aid", "auto")
                } else {
                    mpv.setProperty("aid", value)
                }
            }
        }

        // Volume Step
        Label {
            text: i18n("Volume step")
            Layout.alignment: Qt.AlignRight
        }

        Item {
            height: volumeStep.height
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
            Layout.fillWidth: true
        }

        Item {
            width: Kirigami.Units.gridUnit
            height: Kirigami.Units.gridUnit
        }
    }
}
