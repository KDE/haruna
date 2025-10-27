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

        Label {
            text: i18nc("@label:textbox", "Preferred language")
            Layout.alignment: Qt.AlignRight
        }

        TextField {
            id: preferredLanguage

            text: AudioSettings.preferredLanguage
            placeholderText: "eng, ger etc."
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
            onValueModified: {
                AudioSettings.preferredTrack = value
                AudioSettings.save()
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
            value: AudioSettings.volumeStep
            onValueModified: {
                AudioSettings.volumeStep = volumeStep.value
                AudioSettings.save()
            }
        }

        // Replay Gain
        // See: https://mpv.io/manual/stable/#options-replaygain
        SettingsHeader {
            text: i18nc("@title", "Replay gain")
            Layout.columnSpan: 2
            Layout.fillWidth: true
        }

        Label {
            text: i18nc("@label:listbox", "Mode")
            Layout.alignment: Qt.AlignRight
        }
        RowLayout {
            ComboBox {
                textRole: "text"
                valueRole: "value"
                model: ListModel {
                    id: replayGainModel
                }
                Component.onCompleted: {
                    const noAdjustment = {
                        text: i18nc("@item:listbox replay gain", "Disabled"),
                        value: "no"
                    }
                    replayGainModel.append(noAdjustment)

                    const track = {
                        text: i18nc("@item:listbox replay gain", "Track"),
                        value: "track"
                    }
                    replayGainModel.append(track)

                    const album = {
                        text: i18nc("@item:listbox replay gain", "Album"),
                        value: "album"
                    }
                    replayGainModel.append(album)

                    currentIndex = indexOfValue(AudioSettings.replayGain)
                }

                onActivated: function(index) {
                    AudioSettings.replayGain = model.get(index).value
                    AudioSettings.save()
                }
            }
            ToolTipButton {
                toolTipText: i18nc("@info:tooltip", "Adjust volume gain according to <i>replaygain</i> " +
                                   "values stored in the file metadata. If set to album gain, " +
                                   "apply album gain if present and fallback to track gain otherwise.")
                toolTipWidth: 450

                Layout.preferredHeight: Kirigami.Units.iconSizes.medium
            }
        }

        // Replay Gain Clip
        Item { Layout.preferredWidth: 1 }
        RowLayout {
            CheckBox {
                text: i18nc("@label:spinbox", "Prevent clipping")
                checked: AudioSettings.replayGainPreventClip
                onClicked: {
                    AudioSettings.replayGainPreventClip = checked
                    AudioSettings.save()
                }
            }
            ToolTipButton {
                toolTipText: i18nc("@info:tooltip", "Prevent clipping by lowering the gain")
                toolTipWidth: 450

                Layout.preferredHeight: Kirigami.Units.iconSizes.medium
            }
        }

        // Replay Gain Preamp
        Label {
            text: i18nc("@label:spinbox", "Preamp (dB)")
            Layout.alignment: Qt.AlignRight
        }
        RowLayout {
            id: preamp
            SpinBox {
                editable: true
                from: -150
                to: 150
                value: AudioSettings.replayGainPreamp
                onValueModified: {
                    preamp.setPreamp(value)
                }
            }
            Button {
                flat: true
                icon.name: "edit-reset"
                onClicked: {
                    preamp.setPreamp(0)
                }
                ToolTip {
                    text: i18nc("@info:tooltip", "Reset to default value")
                }
            }

            function setPreamp(val) {
                AudioSettings.replayGainPreamp = val
                AudioSettings.save()
            }
        }

        // Replay Gain Fallback
        Label {
            text: i18nc("@label:spinbox", "Fallback (dB)")
            Layout.alignment: Qt.AlignRight
        }
        RowLayout {
            id: fallback
            SpinBox {
                editable: true
                from: -200
                to: 60
                value: AudioSettings.replayGainFallback
                onValueModified: {
                    fallback.setFallback(value)
                }
            }
            Button {
                flat: true
                icon.name: "edit-reset"
                onClicked: {
                    fallback.setFallback(0)
                }
                ToolTip {
                    text: i18nc("@info:tooltip", "Reset to default value")
                }
            }
            ToolTipButton {
                toolTipText: i18nc("@info:tooltip", "Gain in dB to apply if the file has no replay gain tags.<br><br>" +
                                   "This setting is in effect only when:<br>" +
                                   "- the replaygain setting is disabled<br>" +
                                   "- the replaygain setting is enabled and the file has no replaygain metadata<br>" +
                                   "If this is in effect, other replaygain settings are ignored.")
                toolTipWidth: 450

                Layout.preferredHeight: Kirigami.Units.iconSizes.medium
            }

            function setFallback(val) {
                AudioSettings.replayGainFallback = val
                AudioSettings.save()
            }
        }

        Item {
            Layout.preferredWidth: Kirigami.Units.gridUnit
            Layout.preferredHeight: Kirigami.Units.gridUnit
        }
    }
}
