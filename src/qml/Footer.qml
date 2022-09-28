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
            action: appActions.togglePlaylistAction
        }
    }

    RowLayout {
        id: footerRow
        anchors.fill: parent

        Loader {
            active: !menuBarLoader.visible && !header.visible
            visible: active
            sourceComponent: HamburgerMenu {
                position: HamburgerMenu.Position.Footer
            }
        }

        Loader {
            sourceComponent: togglePlaylistButton
            visible: !PlaylistSettings.canToggleWithMouse && PlaylistSettings.position === "left"
        }

        ToolButton {
            id: playPauseButton
            action: appActions.playPauseAction
            text: ""
            icon.name: mpv.pause ? "media-playback-start" : "media-playback-pause"
            focusPolicy: Qt.NoFocus
            enabled: mpv.duration !== 0

            ToolTip {
                id: playPauseButtonToolTip
                text: mpv.pause ? i18n("Start Playback") : i18n("Pause Playback")
            }
        }

        ToolButton {
            id: playPreviousFile
            action: appActions.playPreviousAction
            icon.name: root.LayoutMirroring.enabled ? "media-skip-forward" : "media-skip-backward"
            text: ""
            focusPolicy: Qt.NoFocus
            enabled: playList.playlistView.count > 1

            ToolTip {
                text: i18n("Play previous file")
            }
        }

        ToolButton {
            id: playNextFile
            action: appActions.playNextAction
            icon.name: root.LayoutMirroring.enabled ? "media-skip-backward" : "media-skip-forward"
            text: ""
            focusPolicy: Qt.NoFocus
            enabled: playList.playlistView.count > 1

            ToolTip {
                text: i18n("Play next file")
            }
        }

        HProgressBar {
            id: progressBar
            Layout.fillWidth: true
        }

        LabelWithTooltip {
            id: timeInfo

            text: app.formatTime(mpv.position) + " / " + app.formatTime(mpv.duration)
            font.pointSize: Math.round(Kirigami.Units.gridUnit * 0.7)
            toolTipText: i18n("Remaining: %1", app.formatTime(mpv.remaining))
            alwaysShowToolTip: true
            horizontalAlignment: Qt.AlignHCenter
        }

        ToolButton {
            id: mute
            action: appActions.muteAction
            icon.name: mpv.mute || mpv.volume === 0 ? "player-volume-muted" : "player-volume"
            text: ""
            focusPolicy: Qt.NoFocus

            ToolTip {
                text: appActions.muteAction.text
            }
        }

        VolumeSlider { id: volume }

        Loader {
            sourceComponent: togglePlaylistButton
            visible: !PlaylistSettings.canToggleWithMouse && PlaylistSettings.position === "right"
        }

    }
}
