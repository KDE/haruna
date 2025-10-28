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
import org.kde.haruna.settings

SettingsBasePage {
    id: root

    required property MpvVideo m_mpv

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
                    text: i18nc("@info:tooltip", "Used for music files that don’t have a video track, "
                                + "an embedded cover image or a cover/folder image "
                                + "in the same folder as the played file.")
                }

                function save() : void {
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
                onRejected: root.m_mpv.focus = true
                onVisibleChanged: {
                    HarunaApp.actionsEnabled = !visible
                }
            }
        }

        SettingsHeader {
            text: i18nc("@title", "Screenshots")
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

        RowLayout {
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

                function save() : void {
                    VideoSettings.screenshotTemplate = text
                    VideoSettings.save()
                }
            }

            ToolTipButton {
                toolTipWidth: 500
                toolTipHeight: 300
                toolTipText: i18nc("@info:tooltip",
                                   "Specify the filename template used to save screenshots. The template specifies the " +
                                   "filename without file extension, and can contain format specifiers, which will be " +
                                   "substituted when taking a screenshot. By default, the template is “%F-%n”. <br>" +
                                   "The template can start with a relative or absolute path, in order to specify " +
                                   "a directory location where screenshots should be saved.<br>" +
                                   "If the final screenshot filename points to an already existing file, the file " +
                                   "will not be overwritten. The screenshot will either not be saved, or if the template " +
                                   "contains %n, saved using different, newly generated filename.<br>" +
                                   "For full info checkout <a href='https://mpv.io/manual/stable/#options-screenshot-template'>https://mpv.io/manual/stable/#options-screenshot-template</a><br><br>" +
                                   "<strong>Allowed format specifiers:</strong><br>" +
                                   "<ul><li><strong>%[#][0X]n</strong> A sequence number, padded with zeros to length X.<br>" +
                                   "<li><strong>%f</strong> Filename of the currently played video.<br>" +
                                   "<li><strong>%F</strong> Same as %f, but without the file extension.<br>" +
                                   "<li><strong>%x</strong> Directory path of the currently played video. "+
                                   "If the video is not on the filesystem (http:// urls), this expands to an empty string.<br>" +
                                   "<li><strong>%X{fallback}</strong> Same as %x, but if the video file is not on the " +
                                   "filesystem, return the fallback string inside the {…}.<br>" +
                                   "<li><strong>%p</strong> Current playback time. The result is a " +
                                   "string of the form “HH:MM:SS”.<br>" +
                                   "<li><strong>%P</strong> Similar to %p, but extended with the playback time in milliseconds “HH:MM:SS.mmm”.<br>" +
                                   "<li><strong>%wX</strong> Specify the current playback time using the format string X.<br>" +
                                   "Valid format specifiers:<br>" +
                                   "<ul><li><strong>%wH</strong> hour (padded with 0 to two digits)</li>" +
                                   "<li><strong>%wh</strong> hour (not padded)</li>" +
                                   "<li><strong>%wM</strong> minutes (00–59)</li>" +
                                   "<li><strong>%wm</strong> total minutes (includes hours, unlike %wM)</li>" +
                                   "<li><strong>%wS</strong> seconds (00–59)</li>" +
                                   "<li><strong>%ws</strong> total seconds (includes hours and minutes)</li>" +
                                   "<li><strong>%wf</strong> like %ws, but as float</li>" +
                                   "<li><strong>%wT</strong> milliseconds (000–999)</li>" +
                                   "<li><strong>%%</strong> Replaced with the % character itself.</li></ul><br>")
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

            value: root.m_mpv.getProperty(MpvProperties.Contrast)
            onSliderValueChanged: function(value) {
                root.m_mpv.setProperty(MpvProperties.Contrast, value.toFixed(0))
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

            value: root.m_mpv.getProperty(MpvProperties.Brightness)
            onSliderValueChanged: function(value) {
                root.m_mpv.setProperty(MpvProperties.Brightness, value.toFixed(0))
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

            value: root.m_mpv.getProperty(MpvProperties.Gamma)
            onSliderValueChanged: function(value) {
                root.m_mpv.setProperty(MpvProperties.Gamma, value.toFixed(0))
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

            value: root.m_mpv.getProperty(MpvProperties.Saturation)
            onSliderValueChanged: function(value) {
                root.m_mpv.setProperty(MpvProperties.Saturation, value.toFixed(0))
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
