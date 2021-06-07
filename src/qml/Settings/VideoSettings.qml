/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

import org.kde.kirigami 2.11 as Kirigami
import org.kde.haruna 1.0
import Haruna.Components 1.0

SettingsBasePage {
    id: root

    hasHelp: true
    helpFile: ":/VideoSettings.html"

    GridLayout {
        id: content

        columns: 2

        SettingsHeader {
            text: qsTr("Screenshots")
            topMargin: 0
            Layout.columnSpan: 2
            Layout.fillWidth: true
        }

        // ------------------------------------
        // Screenshot Format
        // ------------------------------------
        Label {
            text: qsTr("Format")
            Layout.alignment: Qt.AlignRight
        }

        Item {
            height: screenshotFormat.height
            ComboBox {
                id: screenshotFormat
                textRole: "key"
                model: ListModel {
                    ListElement { key: "PNG"; }
                    ListElement { key: "JPG"; }
                    ListElement { key: "WebP"; }
                }

                onActivated: {
                    VideoSettings.screenshotFormat = model.get(index).key
                    VideoSettings.save()
                    mpv.setProperty("screenshot-format", VideoSettings.screenshotFormat)
                }

                Component.onCompleted: {
                    if (VideoSettings.screenshotFormat === "PNG") {
                        currentIndex = 0
                    }
                    if (VideoSettings.screenshotFormat === "JPG") {
                        currentIndex = 1
                    }
                    if (VideoSettings.screenshotFormat === "WebP") {
                        currentIndex = 2
                    }
                }
            }
            Layout.fillWidth: true
        }

        // ------------------------------------
        // Screenshot template
        // ------------------------------------
        Label {
            text: qsTr("Template")
            Layout.alignment: Qt.AlignRight
        }

        Item {
            height: screenshotTemplate.height
            TextField {
                id: screenshotTemplate
                text: VideoSettings.screenshotTemplate
                onEditingFinished: {
                    VideoSettings.screenshotTemplate = text
                    VideoSettings.save()
                    mpv.setProperty("screenshot-template", VideoSettings.screenshotTemplate)
                }
                Layout.fillWidth: true
            }
        }

        SettingsHeader {
            text: qsTr("Image adjustments")
            Layout.columnSpan: 2
            Layout.fillWidth: true
        }


        // ------------------------------------
        // CONTRAST
        // ------------------------------------
        Label {
            text: qsTr("Contrast")
            Layout.alignment: Qt.AlignRight
        }

        ImageAdjustmentSlider {
            id: contrastSlider

            value: mpv.contrast
            onSliderValueChanged: mpv.contrast = value.toFixed(0)

            Layout.topMargin: Kirigami.Units.largeSpacing
        }

        // ------------------------------------
        // BRIGHTNESS
        // ------------------------------------
        Label {
            text: qsTr("Brightness")
            Layout.alignment: Qt.AlignRight
        }

        ImageAdjustmentSlider {
            id: brightnessSlider

            value: mpv.brightness
            onSliderValueChanged: mpv.brightness = value.toFixed(0)

            Layout.topMargin: Kirigami.Units.largeSpacing
        }

        // ------------------------------------
        // GAMMA
        // ------------------------------------
        Label {
            text: qsTr("Gamma")
            Layout.alignment: Qt.AlignRight
        }

        ImageAdjustmentSlider {
            id: gammaSlider

            value: mpv.gamma
            onSliderValueChanged: mpv.gamma = value.toFixed(0)

            Layout.topMargin: Kirigami.Units.largeSpacing
        }

        // ------------------------------------
        // SATURATION
        // ------------------------------------
        Label {
            text: qsTr("Saturation")
            Layout.alignment: Qt.AlignRight
        }

        ImageAdjustmentSlider {
            id: saturationSlider

            value: mpv.saturation
            onSliderValueChanged: mpv.saturation = value.toFixed(0)

            Layout.topMargin: Kirigami.Units.largeSpacing
        }

        Label {
            text: qsTr("Middle click on the sliders to reset them")
            Layout.columnSpan: 2
            Layout.topMargin: Kirigami.Units.largeSpacing
        }

    }
}
