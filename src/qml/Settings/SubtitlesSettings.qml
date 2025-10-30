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
import org.kde.haruna.utilities
import org.kde.haruna.settings

SettingsBasePage {
    id: root

    GridLayout {
        id: content

        columns: 2

        SubtitlesFolders {
            id: subtitleFolders
            implicitWidth: root.width
            Layout.fillWidth: true
            Layout.columnSpan: 2
        }

        Item { Layout.preferredWidth: 1; Layout.preferredHeight: 1 }

        CheckBox {
            id: autoSelectSubtitles

            text: i18nc("@label:check", "Auto select track")
            checked: SubtitlesSettings.autoSelectSubtitles
            
            onClicked: {
                SubtitlesSettings.autoSelectSubtitles = checked
                SubtitlesSettings.save()
            }

            ToolTip {
                text: i18nc("@info:tooltip", "When checked a subtitle track will be automatically selected and displayed")
            }
        }

        Label {
            text: i18nc("@label:textbox", "Preferred language")
            Layout.alignment: Qt.AlignRight
        }

        TextField {
            id: preferredLanguage

            text: SubtitlesSettings.preferredLanguage
            placeholderText: "eng, ger etc."
            Layout.fillWidth: true
            onEditingFinished: save()
            enabled: autoSelectSubtitles.checked

            Connections {
                target: root
                function onSave() {
                    preferredLanguage.save()
                }
            }

            function save() : void {
                SubtitlesSettings.preferredLanguage = text
                SubtitlesSettings.save()
            }
        }

        Label {
            text: i18nc("@label:spinbox", "Preferred track")
            Layout.alignment: Qt.AlignRight
        }

        SpinBox {
            from: 0
            to: 100
            value: SubtitlesSettings.preferredTrack
            editable: true
            enabled: autoSelectSubtitles.checked
            onValueModified: {
                SubtitlesSettings.preferredTrack = value
                SubtitlesSettings.save()
            }
        }

        Item { Layout.preferredWidth: 1; Layout.preferredHeight: 1 }
        CheckBox {
            checked: SubtitlesSettings.allowOnBlackBorders
            text: i18nc("@option:check", "Allow subtitles in black borders")
            onClicked: {
                SubtitlesSettings.allowOnBlackBorders = checked
                SubtitlesSettings.save()
            }

            ToolTip {
                text: i18nc("@info:tooltip", "When checked the subtitles can be rendered outside the video, in the black borders. Might not work for all .ass subtitles.")
            }
        }

        SettingsHeader {
            text: i18nc("@title", "Styling")
            Layout.columnSpan: 2
            Layout.fillWidth: true
        }

        Label {
            text: i18nc("@label:listbox", "Font family")
            Layout.alignment: Qt.AlignRight
        }

        RowLayout {
            Layout.fillWidth: true

            ComboBox {
                id: subtitleFont

                property string defaultFamily: "Sans Serif"

                model: SystemUtils.getFonts()
                onActivated: function(index) {
                    SubtitlesSettings.fontFamily = currentText
                    SubtitlesSettings.save()
                }

                Component.onCompleted: currentIndex = indexOfValue(SubtitlesSettings.fontFamily)
            }

            Button {
                icon.name: "edit-reset"
                onClicked: {
                    const index = subtitleFont.find(subtitleFont.defaultFamily)
                    subtitleFont.currentIndex = index
                }

                ToolTip {
                    text: i18nc("@info:tooltip", "Reset to default value")
                }
            }
        }

        Label {
            text: i18nc("@label:spinbox", "Font size")
            Layout.alignment: Qt.AlignRight
        }

        SpinBox {
            from: 0
            to: 1000
            value: SubtitlesSettings.fontSize
            onValueModified: {
                SubtitlesSettings.fontSize = value
                SubtitlesSettings.save()
            }
        }

        Label {
            text: i18nc("@label", "Font style")
            Layout.alignment: Qt.AlignRight
        }

        CheckBox {
            text: i18nc("@option:check", "Bold")
            checked: SubtitlesSettings.isBold
            onClicked: {
                SubtitlesSettings.isBold = checked
                SubtitlesSettings.save()
            }
        }

        Item { Layout.preferredWidth: 1 }

        CheckBox {
            text: i18nc("@option:check", "Italic")
            checked: SubtitlesSettings.isItalic
            onClicked: {
                SubtitlesSettings.isItalic = checked
                SubtitlesSettings.save()
            }
        }

        Label {
            text: i18nc("@label:textbox", "Font color")
            Layout.alignment: Qt.AlignRight
        }

        RowLayout {
            Layout.fillWidth: true


            ColorPickerButton {
                id: subtitleColorPicker

                color: SubtitlesSettings.fontColor
                onColorChosen: function (color) {
                    subtitleColor.text = color
                }

                Layout.preferredHeight: subtitleColor.height
                Layout.preferredWidth: subtitleColor.height
            }

            TextField {
                id: subtitleColor

                property string defaultColor: "#FFFFFFFF"

                text: SubtitlesSettings.fontColor

                onTextChanged: {
                    save()
                }

                Connections {
                    target: root
                    function onSave() {
                        subtitleColor.save()
                    }
                }

                function save() : void {
                    subtitleColorPicker.color = text
                    SubtitlesSettings.fontColor = text
                    SubtitlesSettings.save()
                }
            }

            Button {
                icon.name: "edit-reset"
                onClicked: subtitleColor.text = subtitleColor.defaultColor

                ToolTip {
                    text: i18nc("@info:tooltip", "Reset to default value")
                }
            }
        }

        Label {
            text: i18nc("@label:textbox", "Shadow color")
            Layout.alignment: Qt.AlignRight
        }

        RowLayout {
            Layout.fillWidth: true

            ColorPickerButton {
                id: shadowColorPicker

                color: SubtitlesSettings.shadowColor
                onColorChosen: function (color) {
                    shadowColor.text = color
                }

                Layout.preferredHeight: shadowColor.height
                Layout.preferredWidth: shadowColor.height
            }

            TextField {
                id: shadowColor

                property string defaultColor: "#80F0F0F0"

                text: SubtitlesSettings.shadowColor

                onTextChanged: {
                    save()
                }

                Connections {
                    target: root
                    function onSave() {
                        shadowColor.save()
                    }
                }

                function save() : void {
                    shadowColorPicker.color = text
                    SubtitlesSettings.shadowColor = text
                    SubtitlesSettings.save()
                }
            }

            Button {
                icon.name: "edit-reset"
                onClicked: shadowColor.text = shadowColor.defaultColor

                ToolTip {
                    text: i18nc("@info:tooltip", "Reset to default value")
                }
            }
        }

        Label {
            text: i18nc("@label:spinbox", "Shadow offset")
            Layout.alignment: Qt.AlignRight
        }

        SpinBox {
            from: 0
            to: 25
            value: SubtitlesSettings.shadowOffset
            editable: true
            onValueModified: {
                SubtitlesSettings.shadowOffset = value
                SubtitlesSettings.save()
            }

            ToolTip {
                text: i18nc("@info:tooltip", "Set to 0 (zero) to disable.")
            }
        }

        Label {
            text: i18nc("@label:textbox", "Border color")
            Layout.alignment: Qt.AlignRight
        }

        RowLayout {
            Layout.fillWidth: true

            ColorPickerButton {
                id: borderColorPicker

                color: SubtitlesSettings.borderColor
                onColorChosen: function (color) {
                    borderColor.text = color
                }

                Layout.preferredHeight: borderColor.height
                Layout.preferredWidth: borderColor.height
            }

            TextField {
                id: borderColor

                property string defaultColor: "#FF000000"

                text: SubtitlesSettings.borderColor

                onTextChanged: {
                    save()
                }

                Connections {
                    target: root
                    function onSave() {
                        borderColor.save()
                    }
                }

                function save() : void {
                    borderColorPicker.color = text
                    SubtitlesSettings.borderColor = text
                    SubtitlesSettings.save()
                }
            }

            Button {
                icon.name: "edit-reset"
                onClicked: borderColor.text = borderColor.defaultColor

                ToolTip {
                    text: i18nc("@info:tooltip", "Reset to default value")
                }
            }
        }

        Label {
            text: i18nc("@label:spinbox", "Border width")
            Layout.alignment: Qt.AlignRight
        }

        SpinBox {
            from: 0
            to: 25
            value: SubtitlesSettings.borderSize
            editable: true
            onValueModified: {
                SubtitlesSettings.borderSize = value
                SubtitlesSettings.save()
            }

            ToolTip {
                text: i18nc("@info:tooltip", "Set to 0 (zero) to disable.")
            }
        }

        Item {
            Layout.preferredWidth: Kirigami.Units.gridUnit
            Layout.preferredHeight: Kirigami.Units.gridUnit
        }
    }
}
