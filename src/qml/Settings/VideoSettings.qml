/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtCore
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Dialogs

import org.kde.kirigami as Kirigami
import org.kde.haruna
import org.kde.haruna.mpvproperties
import org.kde.haruna.settings

SettingsBasePage {
    id: root

    hasHelp: true
    helpFile: ":/VideoSettings.html"
    docPage: "help:/haruna/VideoSettings.html"

    GridLayout {
        id: content

        columns: 2

        Label {
            text: i18nc("@label:listbox", "Default cover")
            Layout.alignment: Qt.AlignRight
        }

        RowLayout {
            TextField {
                id: defaultCover

                text: VideoSettings.defaultCover
                placeholderText: i18nc("placeholder text", "path to image")
                Layout.fillWidth: true
                onEditingFinished: save()

                Connections {
                    target: root
                    function onSave() {
                        defaultCover.save()
                    }
                }

                ToolTip {
                    text: i18nc("@info:tooltip", "Used for music files that don't have a video track, "
                                + "an embedded cover image or a cover/folder image "
                                + "in the same folder as the played file.")
                }

                function save() {
                    VideoSettings.defaultCover = defaultCover.text
                    VideoSettings.save()
                }
            }

            Button {
                icon.name: "folder"
                onClicked: fileDialog.open()
            }

            FileDialog {
                id: fileDialog

                currentFolder: StandardPaths.writableLocation(StandardPaths.PicturesLocation)
                title: i18nc("@title:window", "Select file")
                fileMode: FileDialog.OpenFile

                onAccepted: {
                    VideoSettings.defaultCover = fileDialog.selectedFile
                }
                onRejected: mpv.focus = true
            }
        }

        SettingsHeader {
            text: i18nc("@title", "Screenshots")
            topMargin: 0
            Layout.columnSpan: 2
            Layout.fillWidth: true
        }

        // ------------------------------------
        // Screenshot Format
        // ------------------------------------
        Label {
            text: i18nc("@label:listbox", "Format")
            Layout.alignment: Qt.AlignRight
        }

        ComboBox {
            id: screenshotFormat
            textRole: "key"
            model: ListModel {
                ListElement { key: "PNG"; value: "png" }
                ListElement { key: "JPG"; value: "jpg" }
                ListElement { key: "WebP"; value: "webp" }
                ListElement { key: "JPEG XL"; value: "jxl" }
            }

            onActivated: function(index) {
                VideoSettings.screenshotFormat = model.get(index).value
                VideoSettings.save()
                mpv.setProperty(MpvProperties.ScreenshotFormat, VideoSettings.screenshotFormat)
            }

            Component.onCompleted: {
                if (VideoSettings.screenshotFormat === "png") {
                    currentIndex = 0
                }
                if (VideoSettings.screenshotFormat === "jpg") {
                    currentIndex = 1
                }
                if (VideoSettings.screenshotFormat === "webp") {
                    currentIndex = 2
                }
                if (VideoSettings.screenshotFormat === "jxl") {
                    currentIndex = 3
                }
            }
        }

        // ------------------------------------
        // Screenshot template
        // ------------------------------------
        Label {
            text: i18nc("@label:textbox", "Template")
            Layout.alignment: Qt.AlignRight
        }

        TextField {
            id: screenshotTemplate

            text: VideoSettings.screenshotTemplate
            Layout.fillWidth: true
            onEditingFinished: save()

            Connections {
                target: root
                function onSave() {
                    screenshotTemplate.save()
                }
            }

            function save() {
                VideoSettings.screenshotTemplate = text
                VideoSettings.save()
                mpv.setProperty(MpvProperties.ScreenshotTemplate, VideoSettings.screenshotTemplate)
            }
        }

        SettingsHeader {
            text: i18nc("@title", "Image adjustments")
            Layout.columnSpan: 2
            Layout.fillWidth: true
        }


        // ------------------------------------
        // CONTRAST
        // ------------------------------------
        Label {
            text: i18nc("@label:slider", "Contrast")
            Layout.alignment: Qt.AlignRight
        }

        ImageAdjustmentSlider {
            id: contrastSlider

            value: mpv.getProperty(MpvProperties.Contrast)
            onSliderValueChanged: function(value) {
                mpv.setProperty(MpvProperties.Contrast, value.toFixed(0))
            }

            Layout.topMargin: Kirigami.Units.largeSpacing
        }

        // ------------------------------------
        // BRIGHTNESS
        // ------------------------------------
        Label {
            text: i18nc("@label:slider", "Brightness")
            Layout.alignment: Qt.AlignRight
        }

        ImageAdjustmentSlider {
            id: brightnessSlider

            value: mpv.getProperty(MpvProperties.Brightness)
            onSliderValueChanged: function(value) {
                mpv.setProperty(MpvProperties.Brightness, value.toFixed(0))
            }

            Layout.topMargin: Kirigami.Units.largeSpacing
        }

        // ------------------------------------
        // GAMMA
        // ------------------------------------
        Label {
            text: i18nc("@label:slider", "Gamma")
            Layout.alignment: Qt.AlignRight
        }

        ImageAdjustmentSlider {
            id: gammaSlider

            value: mpv.getProperty(MpvProperties.Gamma)
            onSliderValueChanged: function(value) {
                mpv.setProperty(MpvProperties.Gamma, value.toFixed(0))
            }

            Layout.topMargin: Kirigami.Units.largeSpacing
        }

        // ------------------------------------
        // SATURATION
        // ------------------------------------
        Label {
            text: i18nc("@label:slider", "Saturation")
            Layout.alignment: Qt.AlignRight
        }

        ImageAdjustmentSlider {
            id: saturationSlider

            value: mpv.getProperty(MpvProperties.Saturation)
            onSliderValueChanged: function(value) {
                mpv.setProperty(MpvProperties.Saturation, value.toFixed(0))
            }

            Layout.topMargin: Kirigami.Units.largeSpacing
        }

        Label {
            text: i18nc("@info", "Middle click on the sliders to reset them")
            Layout.columnSpan: 2
            Layout.topMargin: Kirigami.Units.largeSpacing
        }

    }
}
