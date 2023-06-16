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

    property Item m_mpv
    property alias progressBar: progressBar
    property alias footerRow: footerRow
    property alias timeInfo: timeInfo
    property alias playPauseButton: playPauseButton
    property alias volume: volumeSliderLoader.item

    padding: 5
    position: ToolBar.Footer
    hoverEnabled: true

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
            icon.name: m_mpv.pause ? "media-playback-start" : "media-playback-pause"
            focusPolicy: Qt.NoFocus
            enabled: m_mpv.duration !== 0

            ToolTip {
                id: playPauseButtonToolTip
                text: m_mpv.pause ? i18nc("@info:tooltip", "Start Playback") : i18nc("@info:tooltip", "Pause Playback")
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
                text: i18nc("@info:tooltip", "Play previous file")
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
                text: i18nc("@info:tooltip", "Play next file")
            }
        }

        HProgressBar {
            id: progressBar
            Layout.fillWidth: true
        }

        LabelWithTooltip {
            id: timeInfo

            text: m_mpv.formattedPosition + " / " + m_mpv.formattedDuration
            font.pointSize: Math.round(Kirigami.Units.gridUnit * 0.7)
            toolTipText: i18nc("@info:tooltip", "Remaining: %1", m_mpv.formattedRemaining)
            alwaysShowToolTip: true
            horizontalAlignment: Qt.AlignHCenter

            Layout.minimumWidth: textMetrics.width

            TextMetrics {
                id: textMetrics
                font: timeInfo.font
                text: "000:00:00 / 000:00:00"
            }
        }

        ToolButton {
            id: mute
            action: appActions.muteAction
            icon.name: m_mpv.mute || m_mpv.volume === 0 ? "player-volume-muted" : "player-volume"
            text: ""
            focusPolicy: Qt.NoFocus

            ToolTip {
                text: appActions.muteAction.text
            }
        }

        Loader {
            id: volumeSliderLoader

            active: true
            source: app.qtMajorVersion() === 6 ? "VolumeSliderQt6.qml" : "VolumeSlider.qml"
        }

        Loader {
            sourceComponent: togglePlaylistButton
            visible: !PlaylistSettings.canToggleWithMouse && PlaylistSettings.position === "right"
        }

    }
}
