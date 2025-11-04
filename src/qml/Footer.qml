/*
 * SPDX-FileCopyrightText: 2024 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.haruna
import org.kde.haruna.settings

Item {
    id: root

    required property MpvVideo m_mpv
    required property MenuBarLoader m_menuBarLoader
    required property Header m_header
    required property RecentFilesModel m_recentFilesModel
    required property Loader m_settingsLoader

    property alias progressBar: progressBar
    property string footerState: isFloating ? "hidden" : "visible"
    property bool isFloating: GeneralSettings.footerStyle === "floating"
    property bool isSmallSize: width < 600
    property int buttonSize: isSmallSize ? Kirigami.Units.iconSizes.small : Kirigami.Units.iconSizes.smallMedium

    implicitWidth: Window.window.contentItem.width
    implicitHeight: root.isFloating
                    ? footer.height + Kirigami.Units.largeSpacing * (root.isSmallSize ? 1 : 2)
                    : footer.height

    ToolBar {
        id: footer

        x: (root.width - width) / 2
        width: root.isFloating
               ? Math.min(Window.window.contentItem.width - Kirigami.Units.largeSpacing * (root.isSmallSize ? 2 : 4), 900)
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
                action: HarunaApp.actions.togglePlaylistAction
                icon.width: root.buttonSize
                icon.height: root.buttonSize
                display: root.isSmallSize ? AbstractButton.IconOnly : AbstractButton.TextBesideIcon
            }
        }

        ColumnLayout {
            spacing: 0
            anchors.fill: parent

            RowLayout {
                id: footerRow

                spacing: root.isSmallSize ? 1 : Kirigami.Units.smallSpacing

                HamburgerMenu {
                    id: hamburgerMenu

                    m_mpv: root.m_mpv
                    m_recentFilesModel: root.m_recentFilesModel
                    m_settingsLoader: root.m_settingsLoader
                    m_menuBarLoader: root.m_menuBarLoader

                    position: HamburgerMenu.Position.Footer
                    visible: root.m_menuBarLoader.state === "hidden" && !root.m_header.visible
                    icon.width: root.buttonSize
                    icon.height: root.buttonSize
                }

                Loader {
                    sourceComponent: togglePlaylistButton
                    active: !PlaylistSettings.canToggleWithMouse && PlaylistSettings.position === "left"
                    visible: active
                    asynchronous: false
                }

                ToolButton {
                    id: playPauseButton

                    display: AbstractButton.IconOnly
                    icon.name: root.m_mpv.pause ? "media-playback-start" : "media-playback-pause"
                    icon.width: root.buttonSize
                    icon.height: root.buttonSize
                    focusPolicy: Qt.NoFocus
                    enabled: root.m_mpv.duration !== 0

                    onClicked: HarunaApp.actions.playPauseAction.trigger()

                    ToolTip {
                        id: playPauseButtonToolTip

                        text: root.m_mpv.pause ? i18nc("@info:tooltip", "Start playback") : i18nc("@info:tooltip", "Pause playback")
                    }
                }

                ToolButton {
                    id: playPreviousFile

                    icon.name: footer.LayoutMirroring.enabled ? "media-skip-forward" : "media-skip-backward"
                    icon.width: root.buttonSize
                    icon.height: root.buttonSize
                    display: AbstractButton.IconOnly
                    focusPolicy: Qt.NoFocus
                    enabled: root.m_mpv.activeFilterProxyModel.itemCount > 1

                    onClicked: HarunaApp.actions.playPreviousAction.trigger()

                    ToolTip {
                        text: i18nc("@info:tooltip", "Play previous file")
                    }
                }

                ToolButton {
                    id: playNextFile

                    icon.name: footer.LayoutMirroring.enabled ? "media-skip-backward" : "media-skip-forward"
                    icon.width: root.buttonSize
                    icon.height: root.buttonSize
                    display: AbstractButton.IconOnly
                    focusPolicy: Qt.NoFocus
                    enabled: root.m_mpv.activeFilterProxyModel.itemCount > 1

                    onClicked: HarunaApp.actions.playNextAction.trigger()

                    ToolTip {
                        text: i18nc("@info:tooltip", "Play next file")
                    }
                }

                ToolButton {
                    icon.name: "overflow-menu"
                    focusPolicy: Qt.NoFocus
                    visible: !progressBar.showChapterMarkers

                    onClicked: {
                        progressBar.openChapterPopup(this)
                    }

                    ToolTip {
                        text: i18nc("@info:tooltip", "Open chapters menu")
                    }
                }

                HProgressBar {
                    id: progressBar

                    m_mpv: root.m_mpv

                    Layout.fillWidth: true
                    Layout.preferredHeight: 20
                }

                ToolButton {
                    id: mute

                    action: HarunaApp.actions.muteAction
                    icon.name: root.m_mpv.mute || root.m_mpv.volume === 0 ? "player-volume-muted" : "player-volume"
                    icon.width: root.buttonSize
                    icon.height: root.buttonSize
                    display: AbstractButton.IconOnly
                    focusPolicy: Qt.NoFocus

                    ToolTip {
                        text: HarunaApp.actions.muteAction.text
                    }
                }


                VolumeSlider {
                    id: volumeSlider

                    m_mpv: root.m_mpv
                    label.font.pointSize: root.isSmallSize ? 8 : 10

                    Layout.preferredWidth: root.isSmallSize ? 60 : 100
                    Layout.preferredHeight: 20
                }

                Loader {
                    sourceComponent: togglePlaylistButton
                    active: !PlaylistSettings.canToggleWithMouse && PlaylistSettings.position === "right"
                    visible: active
                    asynchronous: false
                }
            }

            RowLayout {
                LabelWithTooltip {
                    id: mediaTitleLabel

                    text: root.m_mpv.mediaTitle
                    font.pointSize: 9
                    toolTipText: root.m_mpv.mediaTitle
                    elide: Text.ElideRight

                    Layout.fillWidth: true
                }

                LabelWithTooltip {
                    id: timeInfo

                    text: "%1 / %2".arg(root.m_mpv.formattedPosition).arg(root.m_mpv.formattedDuration)
                    font.pointSize: 9
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
