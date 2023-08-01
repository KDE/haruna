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
            onActivated: {
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
            onActivated: {
                PlaylistSettings.style = model.get(index).value
                PlaylistSettings.save()
            }
        }

        Item { width: 1; height: 1 }
        CheckBox {
            checked: PlaylistSettings.showToolbar
            text: i18nc("@option:check", "Show toolbar")
            onCheckStateChanged: {
                PlaylistSettings.showToolbar = checked
                PlaylistSettings.save()
            }
        }

        Item { width: 1; height: 1 }
        CheckBox {
            checked: PlaylistSettings.overlayVideo
            text: i18nc("@option:check", "Overlay video")
            onCheckStateChanged: {
                PlaylistSettings.overlayVideo = checked
                PlaylistSettings.save()
            }

            ToolTip {
                text: i18nc("@info:tooltip", "When checked the playlist goes on top of the video\nWhen unchecked the video is resized")
            }
        }

        Item { width: 1; height: 1 }
        CheckBox {
            checked: PlaylistSettings.showMediaTitle
            text: i18nc("@option:check", "Show media title instead of file name")
            onCheckStateChanged: {
                PlaylistSettings.showMediaTitle = checked
                PlaylistSettings.save()
            }
        }

        Item { width: 1; height: 1 }
        CheckBox {
            checked: PlaylistSettings.loadSiblings
            text: i18nc("@option:check", "Auto load videos from same folder")
            onCheckStateChanged: {
                PlaylistSettings.loadSiblings = checked
                PlaylistSettings.save()
            }
        }

        Item { width: 1; height: 1 }
        CheckBox {
            checked: PlaylistSettings.repeat
            text: i18nc("@option:check", "Repeat")
            onCheckStateChanged: {
                PlaylistSettings.repeat = checked
                PlaylistSettings.save()
            }
        }

        Item { width: 1; height: 1 }
        CheckBox {
            checked: PlaylistSettings.showRowNumber
            text: i18nc("@option:check", "Show row number")
            onCheckStateChanged: {
                PlaylistSettings.showRowNumber = checked
                PlaylistSettings.save()
            }
        }

        Item { width: 1; height: 1 }
        CheckBox {
            checked: PlaylistSettings.canToggleWithMouse
            text: i18nc("@option:check", "Toggle with mouse")
            onCheckStateChanged: {
                PlaylistSettings.canToggleWithMouse = checked
                PlaylistSettings.save()
            }
        }

        Item { width: 1; height: 1 }
        CheckBox {
            text: i18nc("@option:check", "Increase font size when fullscreen")
            checked: PlaylistSettings.bigFontFullscreen
            enabled: PlaylistSettings.style === "compact" ? false : true
            onCheckStateChanged: {
                PlaylistSettings.bigFontFullscreen = checked
                PlaylistSettings.save()
                playList.playlistView.forceLayout()
            }
        }

        Item { width: 1; height: 1 }
        CheckBox {
            checked: PlaylistSettings.rememberState
            text: i18nc("@option:check", "Remember last playlist state")
            onCheckStateChanged: {
                PlaylistSettings.rememberState = checked
                PlaylistSettings.save()
            }

            ToolTip {
                text: i18nc("@info:tooltip", "When checked the playlist state (visible/hidden) "
                            + "is remembered across launches")
            }
        }


        Item {
            width: Kirigami.Units.gridUnit
            height: Kirigami.Units.gridUnit
        }
    }
}
