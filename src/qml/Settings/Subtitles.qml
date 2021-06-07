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

        SubtitlesFolders {
            id: subtitleFolders
            implicitWidth: root.width
            Layout.fillWidth: true
            Layout.columnSpan: 2
        }

        Label {
            text: qsTr("Preferred language")
            Layout.alignment: Qt.AlignRight
        }
        TextField {
            text: SubtitlesSettings.preferredLanguage
            placeholderText: "eng,ger etc."
            Layout.fillWidth: true
            onTextEdited: {
                SubtitlesSettings.preferredLanguage = text
                SubtitlesSettings.save()
                mpv.setProperty("slang", text)
            }

            ToolTip {
                text: qsTr("Do not use spaces.")
            }
        }

        Label {
            text: qsTr("Preferred track")
            Layout.alignment: Qt.AlignRight
        }
        SpinBox {
            from: 0
            to: 100
            value: SubtitlesSettings.preferredTrack
            editable: true
            onValueChanged: {
                SubtitlesSettings.preferredTrack = value
                SubtitlesSettings.save()
                if (value === 0) {
                    mpv.setProperty("sid", "auto")
                } else {
                    mpv.setProperty("sid", value)
                }
            }
        }

        Item {
            width: Kirigami.Units.gridUnit
            height: Kirigami.Units.gridUnit
        }
    }
}
