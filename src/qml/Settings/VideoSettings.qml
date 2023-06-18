/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import Qt.labs.platform 1.0 as Platform

import org.kde.kirigami 2.11 as Kirigami
import org.kde.haruna 1.0
import Haruna.Components 1.0

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
                    onSave: defaultCover.save()
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

            Platform.FileDialog {
                id: fileDialog

                folder: Platform.StandardPaths.writableLocation(Platform.StandardPaths.PicturesLocation)
                title: i18nc("@title:window", "Select file")
                fileMode: Platform.FileDialog.OpenFile

                onAccepted: {
                    VideoSettings.defaultCover = fileDialog.file
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

            onActivated: {
                VideoSettings.screenshotFormat = model.get(index).value
                VideoSettings.save()
                mpv.setProperty("screenshot-format", VideoSettings.screenshotFormat)
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
                onSave: screenshotTemplate.save()
            }

            function save() {
                VideoSettings.screenshotTemplate = text
                VideoSettings.save()
                mpv.setProperty("screenshot-template", VideoSettings.screenshotTemplate)
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

            value: mpv.getProperty("contrast")
            onSliderValueChanged: mpv.setProperty("contrast", value.toFixed(0))

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

            value: mpv.getProperty("brightness")
            onSliderValueChanged: mpv.setProperty("brightness", value.toFixed(0))

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

            value: mpv.getProperty("gamma")
            onSliderValueChanged:mpv.setProperty("gamma", value.toFixed(0))

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

            value: mpv.getProperty("saturation")
            onSliderValueChanged: mpv.setProperty("saturation", value.toFixed(0))

            Layout.topMargin: Kirigami.Units.largeSpacing
        }

        Label {
            text: i18nc("@info", "Middle click on the sliders to reset them")
            Layout.columnSpan: 2
            Layout.topMargin: Kirigami.Units.largeSpacing
        }

    }
}
