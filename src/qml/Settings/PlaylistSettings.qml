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
            textRole: "key"
            model: ListModel {
                ListElement { key: "Left"; value: "left" }
                ListElement { key: "Right"; value: "right" }
            }
            Component.onCompleted: {
                for (let i = 0; i < model.count; ++i) {
                    if (model.get(i).value === PlaylistSettings.position) {
                        currentIndex = i
                        break
                    }
                }
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
            textRole: "display"
            model: ListModel {
                ListElement { display: "Default"; value: "default" }
                ListElement { display: "With thumbnails"; value: "withThumbnails" }
                ListElement { display: "Compact"; value: "compact" }
            }
            Component.onCompleted: {
                for (let i = 0; i < model.count; ++i) {
                    if (model.get(i).value === PlaylistSettings.style) {
                        currentIndex = i
                        break
                    }
                }
            }
            onActivated: function(index) {
                PlaylistSettings.style = model.get(index).value
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
            checked: PlaylistSettings.autoplay
            text: i18nc("@option:check", "Autoplay")
            onClicked: {
                PlaylistSettings.autoplay = checked
                PlaylistSettings.save()
            }

            ToolTip {
                text: i18nc("@info:tooltip", "When checked the next video will play automatically")
            }
        }

        Item { Layout.preferredWidth: 1; Layout.preferredHeight: 1 }
        CheckBox {
            enabled: PlaylistSettings.autoplay
            checked: PlaylistSettings.repeat
            text: i18nc("@option:check", "Repeat")
            onClicked: {
                PlaylistSettings.repeat = checked
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
                playlist.playlistView.forceLayout()
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
