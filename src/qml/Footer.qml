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

    required property MpvVideo m_mpv
    required property PlayList m_playlist
    required property MenuBarLoader m_menuBarLoader
    required property Header m_header
    required property RecentFilesModel m_recentFilesModel
    required property Loader m_settingsLoader

    property alias progressBar: progressBar
    property string footerState: isFloating ? "hidden" : "visible"
    property bool isFloating: GeneralSettings.footerStyle === "floating"

    implicitWidth: Window.window.contentItem.width
    implicitHeight: root.isFloating
                    ? footer.height + Kirigami.Units.largeSpacing * 2
                    : footer.height

    ToolBar {
        id: footer

        x: (root.width - width) / 2
        width: root.isFloating
               ? Math.min(Window.window.contentItem.width - Kirigami.Units.largeSpacing * 2, 900)
               : root.width

        state: root.footerState
        padding: Kirigami.Units.smallSpacing
        position: ToolBar.Footer
        hoverEnabled: true
        background: Rectangle {
            color: Qt.alpha(Kirigami.Theme.backgroundColor, root.isFloating ? 0.8 : 1.0)
            radius: root.isFloating ? Kirigami.Units.cornerRadius : 0
        }

        Connections {
            target: HarunaApp
            function onQmlApplicationMouseLeave() {
                if (root.isFloating) {
                    hideFooterTimer.start()
                }
            }
        }

        Connections {
            target: root.m_mpv
            function onMousePositionChanged(x, y) {
                Qt.callLater(() => footer.setState())
            }
        }

        Timer {
            interval: 50
            running: !root.isFloating
            repeat: true
            onTriggered: {
                Qt.callLater(() => footer.staticState())
            }
        }

        Timer {
            id: hideFooterTimer

            running: false
            repeat: false
            interval: 1000
            onTriggered: {
                if (footer.hovered || hamburgerMenu.isOpen || progressBar.chaptersPopupIsOpen) {
                    return
                }

                root.footerState = "hidden"
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

                m_mpv: root.m_mpv
                m_recentFilesModel: root.m_recentFilesModel
                m_settingsLoader: settingsLoader

                position: HamburgerMenu.Position.Footer
                visible: root.m_menuBarLoader.state === "hidden" && !root.m_header.visible
            }

            Loader {
                sourceComponent: togglePlaylistButton
                active: !PlaylistSettings.canToggleWithMouse && PlaylistSettings.position === "left"
                visible: active
                asynchronous: true
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
                active: !PlaylistSettings.canToggleWithMouse && PlaylistSettings.position === "right"
                visible: active
                asynchronous: true
            }
        }

        function setState() : void {
            if (root.isFloating) {
                floatingState()
            } else {
                staticState()
            }
        }

        function floatingState() {
            if (GeneralSettings.floatingFooterTrigger === "EveryMouseMovement") {
                hideFooterTimer.stop()
                root.footerState = "visible"
                if (root.m_mpv.mouseY < root.m_mpv.height - footer.height * 2) {
                    hideFooterTimer.restart()
                }
            } else {
                if (root.m_mpv.mouseY > root.m_mpv.height - footer.height * 2) {
                    const mainWindow = root.Window.window as Main
                    if (mainWindow.containsMouse) {
                        hideFooterTimer.stop()
                        root.footerState = "visible"
                    }
                } else {
                    hideFooterTimer.start()
                }
            }
        }

        function staticState() {
            const mainWindow = Window.window as Main
            if (!mainWindow.isFullScreen() || (root.m_mpv.mouseY > root.m_mpv.height - footer.height * 2 && mainWindow.containsMouse)) {
                root.footerState = "visible"
            } else {
                root.footerState = "hidden"
            }
        }

        states: [
            State {
                name: "hidden"
                PropertyChanges {
                    footer.y: root.height
                    footer.visible: false
                }
            },
            State {
                name : "visible"
                PropertyChanges {
                    footer.y: 0
                    footer.visible: true
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
