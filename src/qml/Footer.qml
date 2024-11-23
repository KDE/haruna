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

    required property MpvVideo m_mpv
    required property PlayList m_playlist
    required property MenuBarLoader m_menuBarLoader
    required property Header m_header
    required property RecentFilesModel m_recentFilesModel
    required property Loader m_settingsLoader

    property alias progressBar: progressBar
    property bool isFloating: false

    state: {
        const mainWindow = Window.window as Main
        if (!mainWindow.isFullScreen() || (root.m_mpv.mouseY > root.m_mpv.height - 4 && mainWindow.containsMouse)) {
            return "visible"
        } else {
            return "hidden"
        }
    }
    implicitWidth: Window.window.contentItem.width
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
            m_mpv: root.m_mpv
            m_recentFilesModel: root.m_recentFilesModel
            m_settingsLoader: settingsLoader

            position: HamburgerMenu.Position.Footer
            visible: root.m_menuBarLoader.state === "hidden" && !root.m_header.visible
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

            m_mpv: root.m_mpv
            Layout.fillWidth: true
        }

        LabelWithTooltip {
            id: timeInfo

            text: root.m_mpv.formattedPosition
                  ? "%1 / %2".arg(root.m_mpv.formattedPosition).arg(root.m_mpv.formattedDuration)
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

            m_mpv: root.m_mpv
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
                root.height: 0
                root.opacity: 0
                root.visible: false
            }
        },
        State {
            name : "visible"
            PropertyChanges {
                root.height: root.implicitHeight
                root.opacity: 1
                root.visible: true
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
