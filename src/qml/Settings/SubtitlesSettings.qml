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
            text: i18n("Preferred language")
            Layout.alignment: Qt.AlignRight
        }
        TextField {
            text: SubtitlesSettings.preferredLanguage
            placeholderText: i18n("eng,ger etc.")
            Layout.fillWidth: true
            onTextEdited: {
                SubtitlesSettings.preferredLanguage = text
                SubtitlesSettings.save()
                mpv.setProperty("slang", text)
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

        SettingsHeader {
            text: i18n("Styling")
            Layout.columnSpan: 2
            Layout.fillWidth: true
        }

        Label {
            text: i18n("Subtitle color")
            Layout.alignment: Qt.AlignRight
        }
        RowLayout {
            Layout.fillWidth: true


            ColorPickerButton {
                id: subtitleColorPicker

                onColorChosen: subtitleColor.text = color
            }

            TextField {
                id: subtitleColor

                text: SubtitlesSettings.subtitleColor
                onTextChanged: {
                    subtitleColorPicker.color = text
                    SubtitlesSettings.subtitleColor = text
                    SubtitlesSettings.save()
                    mpv.setProperty("sub-color", text)
                }
            }

            Button {
                icon.name: "edit-clear-all"
                onClicked: subtitleColor.text = app.getDefaultSubColor()

                ToolTip {
                    text: i18n("Set default value")
                }
            }
        }

        Label {
            text: i18n("Shadow color")
            Layout.alignment: Qt.AlignRight
        }
        RowLayout {
            Layout.fillWidth: true

            ColorPickerButton {
                id: shadowColorPicker

                onColorChosen: shadowColor.text = color
            }

            TextField {
                id: shadowColor

                text: SubtitlesSettings.shadowColor
                onTextChanged: {
                    shadowColorPicker.color = text
                    SubtitlesSettings.shadowColor = text
                    SubtitlesSettings.save()
                    mpv.setProperty("sub-shadow-color", text)
                }
            }

            Button {
                icon.name: "edit-clear-all"
                onClicked: shadowColor.text = app.getDefaultSubShadowColor()

                ToolTip {
                    text: i18n("Set default value")
                }
            }
        }

        Label {
            text: i18n("Shadow offset")
            Layout.alignment: Qt.AlignRight
        }
        SpinBox {
            from: 0
            to: 25
            value: SubtitlesSettings.shadowOffset
            editable: true
            onValueChanged: {
                SubtitlesSettings.shadowOffset = value
                SubtitlesSettings.save()
                mpv.setProperty("sub-shadow-offset", value)
            }

            ToolTip {
                text: i18n("Set to 0 (zero) to disable.")
            }
        }

        Label {
            text: i18n("Border color")
            Layout.alignment: Qt.AlignRight
        }
        RowLayout {
            Layout.fillWidth: true

            ColorPickerButton {
                id: borderColorPicker

                color: SubtitlesSettings.borderColor
                onColorChosen: borderColor.text = color
            }

            TextField {
                id: borderColor

                text: SubtitlesSettings.borderColor
                onTextChanged: {
                    borderColorPicker.color = text
                    SubtitlesSettings.borderColor = text
                    SubtitlesSettings.save()
                    mpv.setProperty("sub-border-color", text)
                }
            }

            Button {
                icon.name: "edit-clear-all"
                onClicked: borderColor.text = app.getDefaultSubBorderColor()

                ToolTip {
                    text: i18n("Set default value")
                }
            }
        }

        Label {
            text: i18n("Border width")
            Layout.alignment: Qt.AlignRight
        }
        SpinBox {
            from: 0
            to: 25
            value: SubtitlesSettings.borderSize
            editable: true
            onValueChanged: {
                SubtitlesSettings.borderSize = value
                SubtitlesSettings.save()
                mpv.setProperty("sub-border-size", value)
            }

            ToolTip {
                text: i18n("Set to 0 (zero) to disable.")
            }
        }

        Item { width: 1 }
        CheckBox {
            text: i18n("Bold")
            checked: SubtitlesSettings.isBold
            onCheckedChanged: {
                SubtitlesSettings.isBold = checked
                SubtitlesSettings.save()
                mpv.setProperty("sub-bold", checked)
            }
        }

        Item { width: 1 }
        CheckBox {
            text: i18n("Italic")
            checked: SubtitlesSettings.isItalic
            onCheckedChanged: {
                SubtitlesSettings.isItalic = checked
                SubtitlesSettings.save()
                mpv.setProperty("sub-italic", checked)
            }
        }

        Item {
            width: Kirigami.Units.gridUnit
            height: Kirigami.Units.gridUnit
        }
    }
}
