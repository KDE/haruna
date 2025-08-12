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
import org.kde.haruna.settings

SettingsBasePage {
    id: root

    GridLayout {
        id: content

        columns: 2

        // Seek Small Step
        Label {
            text: i18nc("@label:spinbox", "Seek small step")
            Layout.alignment: Qt.AlignRight
        }

        RowLayout {

            SpinBox {
                id: seekSmallStep
                editable: true
                from: 0
                to: 100
                value: PlaybackSettings.seekSmallStep
                onValueModified: {
                    PlaybackSettings.seekSmallStep = seekSmallStep.value
                    PlaybackSettings.save()
                }
            }

            ToolTipButton {
                toolTipText: i18nc("@info:tooltip seek small step setting",
                                   "How much to seek when triggering the corresponding action. Seek mode is " +
                                   "<a href='https://mpv.io/manual/stable/#command-interface-seek-<target>-[<flags>]'>exact</a>")
            }
        }

        // Seek Medium Step
        Label {
            text: i18nc("@label:spinbox", "Seek medium step")
            Layout.alignment: Qt.AlignRight
        }

        RowLayout {

            SpinBox {
                id: seekMediumStep
                editable: true
                from: 0
                to: 100
                value: PlaybackSettings.seekMediumStep
                onValueModified: {
                    PlaybackSettings.seekMediumStep = seekMediumStep.value
                    PlaybackSettings.save()
                }
            }

            ToolTipButton {
                toolTipText: i18nc("@info:tooltip seek medium step setting",
                                   "How much to seek when triggering the corresponding action. Seek mode is " +
                                   "<a href='https://mpv.io/manual/stable/#command-interface-seek-<target>-[<flags>]'>exact</a>.<br>" +
                                   "This is also used for mouse wheel seeking, when mouse is over the progress bar.")
            }
        }

        // Seek Big Step
        Label {
            text: i18nc("@label:spinbox", "Seek big step")
            Layout.alignment: Qt.AlignRight
        }

        RowLayout {
            SpinBox {
                id: seekBigStep
                editable: true
                from: 0
                to: 100
                value: PlaybackSettings.seekBigStep
                onValueModified: {
                    PlaybackSettings.seekBigStep = seekBigStep.value
                    PlaybackSettings.save()
                }
            }

            ToolTipButton {
                toolTipText: i18nc("@info:tooltip seek big step setting",
                                   "How much to seek when triggering the corresponding action. Seek mode is " +
                                   "<a href='https://mpv.io/manual/stable/#command-interface-seek-<target>-[<flags>]'>exact</a>")
            }
        }

        Label {
            text: i18nc("@label", "Hardware decoding")
            Layout.alignment: Qt.AlignRight
        }

        RowLayout {
            ComboBox {
                id: hwDecodingComboBox

                textRole: "key"
                model: ListModel {
                    id: hwDecModel
                    ListElement { key: "disabled"; }
                    ListElement { key: "auto"; }
                    ListElement { key: "auto-safe"; }
                    ListElement { key: "vaapi"; }
                    ListElement { key: "nvdec"; }
                    ListElement { key: "cuda"; }
                    ListElement { key: "videotoolbox"; }
                    ListElement { key: "d3d11va"; }
                }

                onActivated: function(index) {
                    PlaybackSettings.hWDecoding = model.get(index).key === "disabled"
                            ? "no"
                            : model.get(index).key
                    PlaybackSettings.save()
                }

                Component.onCompleted: {
                    for (let i = 0; i < hwDecModel.count; ++i) {
                        if (hwDecModel.get(i).key === PlaybackSettings.hWDecoding) {
                            currentIndex = i
                            break
                        }
                    }
                }
            }

            ToolTipButton {
                toolTipWidth: 450
                toolTipHeight: 300
                toolTipText: i18nc("@info:tooltip hardware decoding setting",
                                   "Specify the hardware video decoding API that should be used if possible. " +
                                   "Whether hardware decoding is actually done depends on the video codec. " +
                                   "If hardware decoding is not possible, mpv will fall back on software decoding.<br><br>" +
                                   "<ul><li><strong>auto</strong> tries to automatically enable hardware decoding using the first available method.</li>" +
                                   "<li><strong>auto-safe</strong> similar to auto, but allows only whitelisted methods that are considered “safe”.</li>" +
                                   "<li><strong>vaapi</strong> works with Intel and AMD GPUs only. It also requires the opengl EGL backend.</li>" +
                                   "<li><strong>nvdec</strong> the newest, and recommended method to do hardware decoding on Nvidia GPUs.</li>" +
                                   "<li><strong>cuda</strong> an older implementation of hardware decoding on Nvidia GPUs.</li>" +
                                   "<li><strong>videotoolbox</strong></li>" +
                                   "<li><strong>d3d11va</strong></li>" +
                                   "</ul>" +
                                   "<br><br>" +
                                   "You can read more about hardware decoding here: " +
                                   "<a href='https://mpv.io/manual/stable/#options-hwdec'>https://mpv.io/manual/stable/#options-hwdec</a>" +
                                   "<br><br>" +
                                   "You can set another hardware decoder by creating a startup custom command <strong>set hwdec <i>decoder_name</i></strong>.")
            }
        }

        Item { Layout.preferredWidth: 1 }

        CheckBox {
            id: loadLastPlayedFileCheckBox

            text: i18nc("@option:check", "Open last played file on startup")
            checked: PlaybackSettings.openLastPlayedFile
            onClicked: {
                PlaybackSettings.openLastPlayedFile = checked
                PlaybackSettings.save()
            }

            ToolTip {
                text: i18nc("@info:tooltip open last played file setting",
                            "On startup it opens the file that was playing when the application was closed.")
            }
        }

        Item { Layout.preferredWidth: 1 }

        CheckBox {
            id: pauseOnMinimizeCheckBox

            text: i18nc("@option:check", "Pause on minimize")
            checked: PlaybackSettings.pauseWhileMinimized
            onClicked: {
                PlaybackSettings.pauseWhileMinimized = checked
                PlaybackSettings.save()
            }

            ToolTip {
                text: i18nc("@info:tooltip pause on minimize setting",
                            "Pauses the player while the window is minimized, playback resumes when restored.")
            }
        }

        // ------------------------------------
        // Playback position
        // ------------------------------------

        SettingsHeader {
            text: i18nc("@title", "Playback position")
            Layout.columnSpan: 2
            Layout.fillWidth: true
        }

        Item { Layout.preferredWidth: 1 }

        CheckBox {
            id: saveFilePositionCheckBox

            text: i18nc("@option:check", "Restore playback position")
            checked: PlaybackSettings.restoreFilePosition
            onClicked: {
                PlaybackSettings.restoreFilePosition = checked
                PlaybackSettings.save()
            }
            ToolTip {
                text: i18ncp("@info:tooltip save playback position",
                             // singular
                             "Saves the file position during playback, opening the same file again will seek to the saved position.\n"
                             +"Position is saved every %1 second, except for the last 10 seconds of the video.",
                             // plural
                             "Saves the file position during playback, opening the same file again will seek to the saved position.\n"
                             +"Position is saved every %1 seconds, except for the last 10 seconds of the video.",
                             timePositionSaveInterval.value)
            }
        }

        Item { Layout.preferredWidth: 1 }

        CheckBox {
            id: playOnResumeCheckBox

            text: i18nc("@option:check", "Start playing")
            checked: PlaybackSettings.playOnResume
            enabled: saveFilePositionCheckBox.checked
            onClicked: {
                PlaybackSettings.playOnResume = checked
                PlaybackSettings.save()
            }

            ToolTip {
                text: i18nc("@info:tooltip start playing setting",
                            "The file will be playing after restoring the playback position")
            }
        }

        Item { Layout.preferredWidth: 1 }

        RowLayout {
            enabled: saveFilePositionCheckBox.checked
            SpinBox {
                id: timePositionSaving
                from: 0
                to: 9999
                value: PlaybackSettings.minDurationToSavePosition

                onValueModified: {
                    PlaybackSettings.minDurationToSavePosition = value
                    PlaybackSettings.save()
                }
            }

            Label {
                text: {
                    if (timePositionSaving.value === 0) {
                        return i18nc("@info", "For any duration")
                    } else {
                        return i18ncp("@info",
                                      "If duration is longer than %1 minute",
                                      "If duration is longer than %1 minutes",
                                      timePositionSaving.value)
                    }
                }
                elide: Text.ElideRight
            }
            ToolTipButton {
                toolTipText: i18nc("@info:tooltip restore playback position > duration",
                             "Position for remote files is restored regardless of duration.")
            }
        }

        Item { Layout.preferredWidth: 1 }

        RowLayout {
            enabled: saveFilePositionCheckBox.checked
            SpinBox {
                id: timePositionSaveInterval
                from: 1
                to: 300
                value: PlaybackSettings.savePositionInterval

                onValueModified: {
                    PlaybackSettings.savePositionInterval = value
                    PlaybackSettings.save()
                }
            }

            LabelWithTooltip {
                text: i18ncp("@info:tooltip restore playback position > save interval",
                             "Save position every %1 second",
                             "Save position every %1 seconds",
                             timePositionSaveInterval.value)
                elide: Text.ElideRight
                Layout.fillWidth: true
            }
        }

        // ------------------------------------
        // Chapters
        // ------------------------------------

        SettingsHeader {
            text: i18nc("@title", "Chapters")
            Layout.columnSpan: 2
            Layout.fillWidth: true
        }

        Item { Layout.preferredWidth: 1 }

        CheckBox {
            id: skipChaptersCheckBox
            text: i18nc("@option:check", "Skip chapters")
            checked: PlaybackSettings.skipChapters
            onClicked: {
                PlaybackSettings.skipChapters = checked
                PlaybackSettings.save()
            }

            ToolTip.text: i18nc("@info:tooltip skip chapters setting",
                                "When enabled it automatically skips chapters containing certain words/characters.")
            ToolTip.visible: hovered
            ToolTip.delay: 700
        }

        Item { Layout.preferredWidth: 1 }

        CheckBox {
            text: i18nc("@option:check", "Show OSD message on skip")
            enabled: skipChaptersCheckBox.checked
            checked: PlaybackSettings.showOsdOnSkipChapters
            onClicked: {
                PlaybackSettings.showOsdOnSkipChapters = checked
                PlaybackSettings.save()
            }

            ToolTip.text: i18nc("@info:tooltip show OSD message on skip setting",
                                "When skipping chapters an osd message will show the title of the skipped chapter.")
            ToolTip.visible: hovered
            ToolTip.delay: 700
        }

        Label {
            text: i18nc("@label:textbox", "Keywords")
            enabled: skipChaptersCheckBox.checked
            Layout.alignment: Qt.AlignRight
        }

        TextField {
            id: chaptersToSkip

            text: PlaybackSettings.chaptersToSkip
            placeholderText: i18nc("placeholder text", "op, ed, chapter 1")
            enabled: skipChaptersCheckBox.checked
            Layout.fillWidth: true
            onEditingFinished: save()

            ToolTip.text: i18nc("@info:tooltip skip words setting",
                                "Skip chapters containing these words. Comma separated list. " +
                                "The match is not exact, meaning <strong>op</strong> will match words containing it like <strong><u>op</u>ening</strong>.")
            ToolTip.visible: hovered
            ToolTip.delay: 700

            Connections {
                target: root
                function onSave() {
                    chaptersToSkip.save()
                }
            }

            function save() : void {
                PlaybackSettings.chaptersToSkip = text
                PlaybackSettings.save()
            }
        }


        // ------------------------------------
        // Youtube-dl format settings
        // ------------------------------------

        SettingsHeader {
            text: i18nc("@title", "Youtube-dl")
            Layout.columnSpan: 2
            Layout.fillWidth: true
        }

        Label {
            text: i18n("Format selection")
            Layout.alignment: Qt.AlignRight
        }

        ComboBox {
            id: ytdlFormatComboBox

            textRole: "text"
            valueRole: "value"
            model: ListModel {
                id: formatModel
            }

            onActivated: function(index) {
                if (index === 0) {
                    PlaybackSettings.ytdlFormat = PlaybackSettings.ytdlFormatCustom
                } else {
                    PlaybackSettings.ytdlFormat = ytdlFormatComboBox.valueAt(currentIndex)
                }

                PlaybackSettings.save()
            }

            Component.onCompleted: {
                const customSelection = {
                    text: i18nc("@item:listbox the custom youtube-dl format selection", "Custom"),
                    value: ""
                }
                formatModel.append(customSelection)

                const defaultSelection = {
                    text: i18nc("@item:listbox the default youtube-dl format selection", "Default"),
                    value: "bestvideo+bestaudio/best"
                }
                formatModel.append(defaultSelection)

                formatModel.append({ text: "2160", value: "bestvideo[height<=2160]+bestaudio/best" })
                formatModel.append({ text: "1440", value: "bestvideo[height<=1440]+bestaudio/best" })
                formatModel.append({ text: "1080", value: "bestvideo[height<=1080]+bestaudio/best" })
                formatModel.append({ text: "720",  value: "bestvideo[height<=720]+bestaudio/best" })
                formatModel.append({ text: "480",  value: "bestvideo[height<=480]+bestaudio/best" })

                currentIndex = hIndexOfValue(PlaybackSettings.ytdlFormat)
            }

            ToolTip.text: i18nc("@info:tooltip format selection setting",
                                "Selects a video source that is closest to the selected format.")
            ToolTip.visible: hovered
            ToolTip.delay: 700

            function hIndexOfValue(value) : int {
                switch(value) {
                case "bestvideo+bestaudio/best":
                    return 1
                case "bestvideo[height<=2160]+bestaudio/best":
                    return 2
                case "bestvideo[height<=1440]+bestaudio/best":
                    return 3
                case "bestvideo[height<=1080]+bestaudio/best":
                    return 4
                case "bestvideo[height<=720]+bestaudio/best":
                    return 5
                case "bestvideo[height<=480]+bestaudio/best":
                    return 6
                }
                return 0
            }
        }

        Item { Layout.preferredWidth: 1 }

        TextField {
            id: ytdlCustomFormatField

            text: PlaybackSettings.ytdlFormatCustom
            visible: ytdlFormatComboBox.currentIndex === 0
            placeholderText: "bestvideo+bestaudio/best"

            Layout.fillWidth: true

            onTextChanged: {
                PlaybackSettings.ytdlFormatCustom = ytdlCustomFormatField.text
                PlaybackSettings.ytdlFormat = ytdlCustomFormatField.text
                PlaybackSettings.save()
            }

            Connections {
                target: root
                function onSave() {
                    PlaybackSettings.ytdlFormatCustom = ytdlCustomFormatField.text
                    PlaybackSettings.save()
                }
            }
        }

        Kirigami.SelectableLabel {
            text: PlaybackSettings.ytdlFormat
            visible: ytdlFormatComboBox.currentIndex > 0

            Layout.fillWidth: true
        }

        // ------------------------------------
        // END - Youtube-dl format settings
        // ------------------------------------

        Item {
            Layout.preferredWidth: Kirigami.Units.gridUnit
            Layout.preferredHeight: Kirigami.Units.gridUnit
        }
    }
}
