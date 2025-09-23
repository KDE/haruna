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
            text: i18nc("@label:listbox", "Position")
            Layout.alignment: Qt.AlignRight
        }

        ComboBox {
            textRole: "text"
            valueRole: "value"
            model: ListModel {
                id: positionModel
            }
            Component.onCompleted: {
                const left = {
                    text: i18nc("@item:listbox playlist position", "Left"),
                    value: "left"
                }
                positionModel.append(left)

                const right = {
                    text: i18nc("@item:listbox playlist position", "Right"),
                    value: "right"
                }
                positionModel.append(right)

                currentIndex = indexOfValue(PlaylistSettings.position)
            }

            onActivated: function(index) {
                PlaylistSettings.position = model.get(index).value
                PlaylistSettings.save()
            }
        }

        Label {
            text: i18nc("@label:listbox", "Playlist style")
            Layout.alignment: Qt.AlignRight
        }

        ComboBox {
            textRole: "text"
            valueRole: "value"
            model: ListModel {
                id: styleModel
            }
            Component.onCompleted: {
                const defaultStyle = {
                    text: i18nc("@item:listbox the style of the playlist", "Default"),
                    value: "default"
                }
                styleModel.append(defaultStyle)

                const thumbnail = {
                    text: i18nc("@item:listbox the style of the playlist", "With thumbnails"),
                    value: "withThumbnails"
                }
                styleModel.append(thumbnail)

                const compact = {
                    text: i18nc("@item:listbox the style of the playlist", "Compact"),
                    value: "compact"
                }
                styleModel.append(compact)

                currentIndex = indexOfValue(PlaylistSettings.style)
            }
            onActivated: function(index) {
                PlaylistSettings.style = model.get(index).value
                PlaylistSettings.save()
            }
        }

        Label {
            text: i18nc("@label:listbox", "Playback behavior")
            Layout.alignment: Qt.AlignRight
        }

        RowLayout {
            ComboBox {
                id: playbackBehaviorComboBox
                textRole: "text"
                valueRole: "value"
                model: ListModel {
                    id: playbackBehaviorModel
                }
                Component.onCompleted: {
                    const repeat = {
                        text: i18nc("@item:listbox the playback behavior of the playlist", "Repeat playlist"),
                        value: "RepeatPlaylist"
                    }
                    playbackBehaviorModel.append(repeat)

                    const stopAfterLast = {
                        text: i18nc("@item:listbox the playback behavior of the playlist", "Stop after last item"),
                        value: "StopAfterLast"
                    }
                    playbackBehaviorModel.append(stopAfterLast)

                    const repeatItem = {
                        text: i18nc("@item:listbox the playback behavior of the playlist", "Repeat item"),
                        value: "RepeatItem"
                    }
                    playbackBehaviorModel.append(repeatItem)

                    const stopAfterItem = {
                        text: i18nc("@item:listbox the playback behavior of the playlist", "Stop after item"),
                        value: "StopAfterItem"
                    }
                    playbackBehaviorModel.append(stopAfterItem)

                    currentIndex = indexOfValue(PlaylistSettings.playbackBehavior)
                }
                onActivated: function(index) {
                    PlaylistSettings.playbackBehavior = model.get(index).value
                    PlaylistSettings.save()
                }
                // When changed from mpris or toolbar, sync the GUI:
                Connections {
                    target: PlaylistSettings
                    function onPlaybackBehaviorChanged() {
                        const comboBox = playbackBehaviorComboBox
                        comboBox.currentIndex = comboBox.indexOfValue(PlaylistSettings.playbackBehavior)
                    }
                }
            }

            ToolTipButton {
                toolTipText: i18nc("@info:tooltip playback description",
                                   "<strong>Repeat playlist</strong>: playlist is repeated indefinitely<br>" +
                                   "<strong>Stop after last item</strong>: playback stops after the last item in the playlist<br>" +
                                   "<strong>Repeat item</strong>: current playing item is repeated indefinitely<br>" +
                                   "<strong>Stop after item</strong>: playback stops after the current playing item")
                toolTipWidth: Math.min(450, root.width)
            }
        }

        Item { Layout.preferredWidth: 1; Layout.preferredHeight: 1 }
        CheckBox {
            checked: PlaylistSettings.randomPlayback
            enabled: ["StopAfterLast", "RepeatPlaylist"].includes(PlaylistSettings.playbackBehavior)
            text: i18nc("@item:listbox the playback behavior of the playlist", "Random playback")
            onClicked: {
                PlaylistSettings.randomPlayback = checked
                PlaylistSettings.save()
            }
        }

        Item { Layout.preferredWidth: 1; Layout.preferredHeight: 1 }
        CheckBox {
            checked: PlaylistSettings.showToolbar
            text: i18nc("@option:check", "Show toolbar")
            onClicked: {
                PlaylistSettings.showToolbar = checked
                PlaylistSettings.save()
            }
        }

        Item { Layout.preferredWidth: 1; Layout.preferredHeight: 1 }
        CheckBox {
            checked: PlaylistSettings.overlayVideo
            text: i18nc("@option:check", "Overlay video")
            onClicked: {
                PlaylistSettings.overlayVideo = checked
                PlaylistSettings.save()
            }

            ToolTip {
                text: i18nc("@info:tooltip", "When checked the playlist goes on top of the video\nWhen unchecked the video is resized")
            }
        }

        Item { Layout.preferredWidth: 1; Layout.preferredHeight: 1 }
        CheckBox {
            checked: PlaylistSettings.showMediaTitle
            text: i18nc("@option:check", "Show media title instead of file name")
            onClicked: {
                PlaylistSettings.showMediaTitle = checked
                PlaylistSettings.save()
            }
        }

        Item { Layout.preferredWidth: 1; Layout.preferredHeight: 1 }
        CheckBox {
            checked: PlaylistSettings.loadSiblings
            text: i18nc("@option:check", "Auto load videos from same folder")
            onClicked: {
                PlaylistSettings.loadSiblings = checked
                PlaylistSettings.save()
            }
        }

        Item { Layout.preferredWidth: 1; Layout.preferredHeight: 1 }
        CheckBox {
            checked: PlaylistSettings.showRowNumber
            text: i18nc("@option:check", "Show row number")
            onClicked: {
                PlaylistSettings.showRowNumber = checked
                PlaylistSettings.save()
            }
        }

        Item { Layout.preferredWidth: 1; Layout.preferredHeight: 1 }
        CheckBox {
            checked: PlaylistSettings.canToggleWithMouse
            text: i18nc("@option:check", "Toggle with mouse")
            onClicked: {
                PlaylistSettings.canToggleWithMouse = checked
                PlaylistSettings.save()
            }
        }

        Item { Layout.preferredWidth: 1; Layout.preferredHeight: 1 }
        CheckBox {
            text: i18nc("@option:check", "Increase font size when fullscreen")
            checked: PlaylistSettings.bigFontFullscreen
            enabled: PlaylistSettings.style === "compact" ? false : true
            onClicked: {
                PlaylistSettings.bigFontFullscreen = checked
                PlaylistSettings.save()
            }
        }

        Item { Layout.preferredWidth: 1; Layout.preferredHeight: 1 }
        CheckBox {
            checked: PlaylistSettings.rememberState
            text: i18nc("@option:check", "Remember last playlist state")
            onClicked: {
                PlaylistSettings.rememberState = checked
                PlaylistSettings.save()
            }

            ToolTip {
                text: i18nc("@info:tooltip", "When checked the playlist state (visible/hidden) "
                            + "is remembered across launches")
            }
        }

        Item { Layout.preferredWidth: 1; Layout.preferredHeight: 1 }
        CheckBox {
            checked: PlaylistSettings.openWithSingleClick
            text: i18nc("@option:check", "Open items with single click")
            onClicked: {
                PlaylistSettings.openWithSingleClick = checked
                PlaylistSettings.save()
            }
        }

        Item {
            Layout.preferredWidth: Kirigami.Units.gridUnit
            Layout.preferredHeight: Kirigami.Units.gridUnit
        }
    }
}
