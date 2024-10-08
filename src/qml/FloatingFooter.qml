/*
 * SPDX-FileCopyrightText: 2024 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.haruna
import org.kde.haruna.settings

Item {
    id: root

    required property Kirigami.ApplicationWindow m_window
    required property MpvVideo m_mpv
    required property PlayList m_playlist

    property alias progressBar: progressBar
    property bool isFloating: true

    implicitWidth: m_window.contentItem.width
    implicitHeight: footer.height + Kirigami.Units.largeSpacing * 2

    ToolBar {
        id: footer

        function getState() {
            if (root.m_mpv.mouseY > root.m_mpv.height - footer.height * 2) {
                if (root.m_window.isFullScreen() && root.m_window.containsMouse) {
                    return "visible"
                }
                if (root.m_window.isFullScreen() && !root.m_window.containsMouse){
                    return "hidden"
                }
                return "visible"
            }
            return "hidden"
        }

        x: (root.width - width) / 2
        width: Math.min(root.m_window.contentItem.width - Kirigami.Units.largeSpacing * 2, 900)

        state: GeneralSettings.floatingFooterTrigger === "EveryMouseMovement" ? "hidden" : getState()
        padding: Kirigami.Units.smallSpacing
        position: ToolBar.Footer
        hoverEnabled: true
        background: Rectangle {
            color: Qt.alpha(Kirigami.Theme.backgroundColor, 0.8)
            radius: Kirigami.Units.cornerRadius
        }

        Connections {
            target: GeneralSettings
            function onFloatingFooterTriggerChanged() {
                if (GeneralSettings.floatingFooterTrigger === "BottomMouseMovement") {
                    footer.state = Qt.binding(() => footer.getState())
                }
            }
        }

        Connections {
            target: root.m_mpv
            function onMousePositionChanged(x, y) {
                if (GeneralSettings.floatingFooterTrigger === "EveryMouseMovement") {
                    footer.state = "visible"
                    hideFooterTimer.restart()
                }
            }
        }

        Timer {
            id: hideFooterTimer

            running: GeneralSettings.floatingFooterTrigger === "EveryMouseMovement"
            repeat: false
            interval: 1000
            onTriggered: {
                if (footer.hovered || hamburgerMenu.isOpen) {
                    return
                }
                footer.state = "hidden"
            }
        }

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
                id: hamburgerMenu

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
                icon.name: root.m_mpv.pause ? "media-playback-start" : "media-playback-pause"
                focusPolicy: Qt.NoFocus
                enabled: root.m_mpv.duration !== 0

                ToolTip {
                    id: playPauseButtonToolTip

                    text: root.m_mpv.pause ? i18nc("@info:tooltip", "Start Playback") : i18nc("@info:tooltip", "Pause Playback")
                }
            }

            ToolButton {
                id: playPreviousFile

                action: appActions.playPreviousAction
                icon.name: footer.LayoutMirroring.enabled ? "media-skip-forward" : "media-skip-backward"
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
                icon.name: footer.LayoutMirroring.enabled ? "media-skip-backward" : "media-skip-forward"
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

                text: root.m_mpv.formattedPosition
                      ? root.m_mpv.formattedPosition + " / " + root.m_mpv.formattedDuration
                      : "00:00:00 / 00:00:00"
                font.pointSize: Math.floor(Kirigami.Units.gridUnit * 0.6)
                toolTipText: i18nc("@info:tooltip", "Remaining: %1", root.m_mpv.formattedRemaining)
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
                icon.name: root.m_mpv.mute || root.m_mpv.volume === 0 ? "player-volume-muted" : "player-volume"
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
                    target: footer
                    y: root.height
                    visible: false
                }
            },
            State {
                name : "visible"
                PropertyChanges {
                    target: footer
                    y: 0
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
                            target: footer
                            property: "y"
                            duration: Kirigami.Units.shortDuration
                            easing.type: Easing.Linear
                        }
                    }
                    PropertyAction {
                        target: footer
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
                        target: footer
                        property: "visible"
                        value: true
                    }
                    ParallelAnimation {
                        NumberAnimation {
                            target: footer
                            property: "y"
                            duration: Kirigami.Units.shortDuration
                            easing.type: Easing.Linear
                        }
                    }
                }
            }
        ]
    }
}
