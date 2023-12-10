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
import Haruna.Components

ToolBar {
    id: root

    property Item m_mpv
    property alias progressBar: progressBar

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

        Loader {
            asynchronous: true
            visible: menuBarLoader.state === "hidden" && !header.visible
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
            enabled: playlist.playlistView.count > 1

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
            enabled: playlist.playlistView.count > 1

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
