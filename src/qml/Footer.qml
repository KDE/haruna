/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.haruna
import org.kde.haruna.settings

ToolBar {
    id: root

    required property Kirigami.ApplicationWindow m_window
    required property MpvVideo m_mpv
    required property PlayList m_playlist

    property alias progressBar: progressBar
    property bool isFloating: false

    state: !m_window.isFullScreen() || (m_mpv.mouseY > m_mpv.height - 4 && m_window.containsMouse)
           ? "visible" : "hidden"
    implicitWidth: m_window.contentItem.width
    padding: Kirigami.Units.smallSpacing
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

        HamburgerMenu {
            position: HamburgerMenu.Position.Footer
            visible: menuBarLoader.state === "hidden" && !header.visible
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
            enabled: root.m_playlist.playlistView.count > 1

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
            enabled: root.m_playlist.playlistView.count > 1

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

            text: m_mpv.formattedPosition
                  ? m_mpv.formattedPosition + " / " + m_mpv.formattedDuration
                  : "00:00:00 / 00:00:00"
            font.pointSize: Math.floor(Kirigami.Units.gridUnit * 0.6)
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

        VolumeSlider {
            id: volumeSlider

            m_mpv: mpv
        }

        Loader {
            sourceComponent: togglePlaylistButton
            visible: !PlaylistSettings.canToggleWithMouse && PlaylistSettings.position === "right"
        }

    }

    states: [
        State {
            name: "hidden"
            PropertyChanges {
                target: root
                height: 0
                opacity: 0
                visible: false
            }
        },
        State {
            name : "visible"
            PropertyChanges {
                target: root
                height: root.implicitHeight
                opacity: 1
                visible: true
            }
        }
    ]

    transitions: [
        Transition {
            from: "visible"
            to: "hidden"

            SequentialAnimation {
                ParallelAnimation {
                    NumberAnimation {
                        target: root
                        property: "opacity"
                        duration: Kirigami.Units.shortDuration
                        easing.type: Easing.Linear
                    }
                    NumberAnimation {
                        target: root
                        property: "height"
                        duration: Kirigami.Units.shortDuration
                        easing.type: Easing.Linear
                    }
                }
                PropertyAction {
                    target: root
                    property: "visible"
                    value: false
                }
            }
        },
        Transition {
            from: "hidden"
            to: "visible"

            SequentialAnimation {
                PropertyAction {
                    target: root
                    property: "visible"
                    value: true
                }
                ParallelAnimation {
                    NumberAnimation {
                        target: root
                        property: "height"
                        duration: Kirigami.Units.shortDuration
                        easing.type: Easing.Linear
                    }
                    NumberAnimation {
                        target: root
                        property: "opacity"
                        duration: Kirigami.Units.shortDuration
                        easing.type: Easing.Linear
                    }
                }
            }
        }
    ]
}
