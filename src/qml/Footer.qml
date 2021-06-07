/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import org.kde.kirigami 2.11 as Kirigami
import org.kde.haruna 1.0
import Haruna.Components 1.0

ToolBar {
    id: root

    property alias progressBar: progressBar
    property alias footerRow: footerRow
    property alias timeInfo: timeInfo
    property alias playPauseButton: playPauseButton
    property alias volume: volume

    anchors.left: parent.left
    anchors.right: parent.right
    anchors.bottom: isFullScreen() ? mpv.bottom : parent.bottom
    padding: 5
    position: ToolBar.Footer
    hoverEnabled: true
    visible: !window.isFullScreen() || mpv.mouseY > window.height - footer.height

    Component {
        id: togglePlaylistButton

        ToolButton {
            action: actions.togglePlaylistAction
        }
    }

    RowLayout {
        id: footerRow
        anchors.fill: parent

        ToolButton {
            icon.name: "application-menu"
            visible: !menuBar.visible
            focusPolicy: Qt.NoFocus
            onClicked: {
                if (mpvContextMenu.visible) {
                    return
                }

                mpvContextMenu.visible = !mpvContextMenu.visible
                const menuHeight = mpvContextMenu.count * mpvContextMenu.itemAt(0).height
                mpvContextMenu.popup(footer, 0, -menuHeight)
            }
        }

        Loader {
            sourceComponent: togglePlaylistButton
            visible: !PlaylistSettings.canToggleWithMouse && PlaylistSettings.position === "left"
        }

        ToolButton {
            id: playPauseButton
            action: actions.playPauseAction
            text: ""
            icon.name: "media-playback-start"
            focusPolicy: Qt.NoFocus

            ToolTip {
                id: playPauseButtonToolTip
                text: mpv.pause ? qsTr("Start Playback") : qsTr("Pause Playback")
            }
        }

        ToolButton {
            id: playPreviousFile
            action: actions.playPreviousAction
            text: ""
            focusPolicy: Qt.NoFocus
            enabled: playList.playlistView.count > 1

            ToolTip {
                text: qsTr("Play previous file")
            }
        }

        ToolButton {
            id: playNextFile
            action: actions.playNextAction
            text: ""
            focusPolicy: Qt.NoFocus
            enabled: playList.playlistView.count > 1

            ToolTip {
                text: qsTr("Play next file")
            }
        }

        HProgressBar {
            id: progressBar
            Layout.fillWidth: true
        }

        LabelWithTooltip {
            id: timeInfo

            text: app.formatTime(mpv.position) + " / " + app.formatTime(mpv.duration)
            font.pointSize: Kirigami.Units.gridUnit - 4
            toolTipText: qsTr("Remaining: ") + app.formatTime(mpv.remaining)
            toolTipFontSize: timeInfo.font.pointSize + 2
            alwaysShowToolTip: true
            horizontalAlignment: Qt.AlignHCenter
        }

        ToolButton {
            id: mute
            action: actions.muteAction
            text: ""
            focusPolicy: Qt.NoFocus

            ToolTip {
                text: actions.muteAction.text
            }
        }

        VolumeSlider { id: volume }

        Loader {
            sourceComponent: togglePlaylistButton
            visible: !PlaylistSettings.canToggleWithMouse && PlaylistSettings.position === "right"
        }

    }
}
