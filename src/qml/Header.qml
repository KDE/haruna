/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQml

import org.kde.kirigami as Kirigami
import org.kde.haruna
import org.kde.haruna.settings

ToolBar {
    id: root

    required property MpvVideo m_mpv
    required property MenuBarLoader m_menuBarLoader
    required property RecentFilesModel m_recentFilesModel
    required property Loader m_settingsLoader

    property bool isSmallSize: width < 600
    property int buttonSize: isSmallSize ? Kirigami.Units.iconSizes.small : Kirigami.Units.iconSizes.smallMedium

    position: ToolBar.Header
    state: {
        const mainWindow = Window.window as Main
        if (!mainWindow.isFullScreen() && GeneralSettings.showHeader) {
            return "visible"
        } else {
            return "hidden"
        }
    }

    onVisibleChanged: {
        const mainWindow = Window.window as Main
        mainWindow.resizeWindow()
    }

    RowLayout {
        id: headerRow

        width: parent.width

        HamburgerMenu {
            m_mpv: root.m_mpv
            m_recentFilesModel: root.m_recentFilesModel
            m_settingsLoader: root.m_settingsLoader
            m_menuBarLoader: root.m_menuBarLoader

            position: HamburgerMenu.Position.Header
            visible: !root.m_menuBarLoader.visible || root.m_menuBarLoader.isShowAnimationRunning

            icon.width: root.buttonSize
            icon.height: root.buttonSize
        }

        ToolButton {
            action: HarunaApp.actions.openFileAction
            focusPolicy: Qt.NoFocus
            icon.width: root.buttonSize
            icon.height: root.buttonSize
        }

        ToolButton {
            action: HarunaApp.actions.openUrlAction
            focusPolicy: Qt.NoFocus
            icon.width: root.buttonSize
            icon.height: root.buttonSize
        }

        ToolSeparator {}

        ToolButton {
            id: subtitleMenuButton

            text: i18nc("@action:intoolbar", "Subtitles")
            icon.name: "add-subtitle"
            icon.width: root.buttonSize
            icon.height: root.buttonSize
            focusPolicy: Qt.NoFocus
            enabled: root.m_mpv.subtitleTracksModel.rowCount > 1
            opacity: enabled ? 1.0 : 0.6
            display: root.isSmallSize ? AbstractButton.IconOnly : AbstractButton.TextBesideIcon

            onReleased: {
                subtitleMenu.visible = !subtitleMenu.visible
            }

            SubtitleTracksMenu {
                id: subtitleMenu

                m_mpv: root.m_mpv
                isPrimarySubtitleMenu: true

                y: parent.height
            }
        }

        ToolButton {
            text: i18nc("@action:intoolbar", "Audio")
            icon.name: "audio-volume-high"
            icon.width: root.buttonSize
            icon.height: root.buttonSize
            focusPolicy: Qt.NoFocus
            enabled: root.m_mpv.audioTracksModel.rowCount > 0
            opacity: enabled ? 1.0 : 0.6
            display: root.isSmallSize ? AbstractButton.IconOnly : AbstractButton.TextBesideIcon

            onReleased: {
                audioMenu.visible = !audioMenu.visible
            }

            AudioTracksMenu {
                id: audioMenu

                m_mpv: root.m_mpv

                y: parent.height
                model: root.m_mpv.audioTracksModel
            }
        }

        Item {
            Layout.fillWidth: true
        }

        ToolButton {
            action: HarunaApp.actions.configureAction
            focusPolicy: Qt.NoFocus
            icon.width: root.buttonSize
            icon.height: root.buttonSize
            display: root.isSmallSize ? AbstractButton.IconOnly : AbstractButton.TextBesideIcon
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
                        duration: Kirigami.Units.veryShortDuration
                        easing.type: Easing.Linear
                    }
                    NumberAnimation {
                        target: root
                        property: "height"
                        duration: Kirigami.Units.veryShortDuration
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
                        duration: Kirigami.Units.veryShortDuration
                        easing.type: Easing.Linear
                    }
                    NumberAnimation {
                        target: root
                        property: "opacity"
                        duration: Kirigami.Units.veryShortDuration
                        easing.type: Easing.Linear
                    }
                }
            }
        }
    ]
}
