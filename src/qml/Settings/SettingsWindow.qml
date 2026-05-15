/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Window
import QtQuick.Controls

import org.kde.kirigami as Kirigami
import org.kde.coreaddons as KCoreAddons
import org.kde.haruna

Loader {
    id: root

    required property MpvVideo m_mpv
    required property ProxyActionsModel m_proxyActionsModel
    required property CustomCommandsModel m_customCommandsModel
    required property MouseActionsModel m_mouseActionsModel

    property int activePage: SettingsWindow.Page.General

    enum Page {
        General = 0,
        Playback,
        Video,
        Audio,
        Subtitles,
        Playlist,
        Mouse,
        Shortcuts,
        CustomCommands,
        Debug,
        About
    }

    function openSettingPage(page: int) : void {
        if (!root.active) {
            root.active = true
            root.loaded.connect(function() {
                root.openSettingPage(page)
            })
            return
        }

        const settingsWindow = root.item as Kirigami.ApplicationWindow
        root.activePage = page
        if (settingsWindow.visible) {
            settingsWindow.raise()
        } else {
            settingsWindow.visible = true
        }
    }

    active: false
    asynchronous: true
    sourceComponent: Kirigami.ApplicationWindow {
        id: settingsWindow

        property string settingsPath: "qrc:/qt/qml/org/kde/haruna/qml/Settings"

        width: 800
        height: 600
        minimumWidth: 750
        minimumHeight: 400
        title: i18nc("@title:window", "Settings")
        transientParent: null
        pageStack.initialPage: Kirigami.ScrollablePage {
            globalToolBarStyle: Kirigami.ApplicationHeaderStyle.None
            bottomPadding: 0
            leftPadding: 0
            rightPadding: 0
            topPadding: 0
            Kirigami.Theme.colorSet: Kirigami.Theme.View

            ListView {
                id: settingsPagesList

                currentIndex: root.activePage

                property list<Kirigami.Action> pages: [
                    Kirigami.Action {
                        text: i18nc("@action", "General")
                        icon.name: "configure"
                        onTriggered: {
                            root.activePage = SettingsWindow.Page.General
                            settingsWindow.pageStack.removePage(1)
                            settingsWindow.pageStack.push(`${settingsWindow.settingsPath}/GeneralSettings.qml`)
                        }
                    },
                    Kirigami.Action {
                        text: i18nc("@action", "Playback")
                        icon.name: "media-playback-start"
                        onTriggered: {
                            root.activePage = SettingsWindow.Page.Playback
                            settingsWindow.pageStack.removePage(1)
                            settingsWindow.pageStack.push(`${settingsWindow.settingsPath}/PlaybackSettings.qml`)
                        }
                    },
                    Kirigami.Action {
                        text: i18nc("@action", "Video")
                        icon.name: "video-x-generic"
                        onTriggered: {
                            root.activePage = SettingsWindow.Page.Video
                            settingsWindow.pageStack.removePage(1)
                            settingsWindow.pageStack.push(`${settingsWindow.settingsPath}/VideoSettings.qml`,
                                                               {m_mpv: root.m_mpv})
                        }
                    },
                    Kirigami.Action {
                        text: i18nc("@action", "Audio")
                        icon.name: "player-volume"
                        onTriggered: {
                            root.activePage = SettingsWindow.Page.Audio
                            settingsWindow.pageStack.removePage(1)
                            settingsWindow.pageStack.push(`${settingsWindow.settingsPath}/AudioSettings.qml`)
                        }
                    },
                    Kirigami.Action {
                        text: i18nc("@action", "Subtitles")
                        icon.name: "add-subtitle"
                        onTriggered: {
                            root.activePage = SettingsWindow.Page.Subtitles
                            settingsWindow.pageStack.removePage(1)
                            settingsWindow.pageStack.push(`${settingsWindow.settingsPath}/SubtitlesSettings.qml`)
                        }
                    },
                    Kirigami.Action {
                        text: i18nc("@action", "Playlist")
                        icon.name: "view-media-playlist"
                        onTriggered: {
                            root.activePage = SettingsWindow.Page.Playlist
                            settingsWindow.pageStack.removePage(1)
                            settingsWindow.pageStack.push(`${settingsWindow.settingsPath}/PlaylistSettings.qml`)
                        }
                    },
                    Kirigami.Action {
                        text: i18nc("@action", "Mouse")
                        icon.name: "input-mouse"
                        onTriggered: {
                            root.activePage = SettingsWindow.Page.Mouse
                            settingsWindow.pageStack.removePage(1)
                            settingsWindow.pageStack.push(`${settingsWindow.settingsPath}/MouseSettings.qml`,
                                                {m_mouseActionsModel: root.m_mouseActionsModel})
                        }
                    },
                    Kirigami.Action {
                        text: i18nc("@action", "Shortcuts")
                        icon.name: "configure-shortcuts"
                        onTriggered: {
                            root.activePage = SettingsWindow.Page.Shortcuts
                            settingsWindow.pageStack.removePage(1)
                            settingsWindow.pageStack.push(`${settingsWindow.settingsPath}/ShortcutsSettings.qml`,
                                                               {m_proxyActionsModel: root.m_proxyActionsModel})
                        }
                    },
                    Kirigami.Action {
                        text: i18nc("@action", "Custom Commands")
                        icon.name: "configure"
                        onTriggered: {
                            root.activePage = SettingsWindow.Page.CustomCommands
                            settingsWindow.pageStack.removePage(1)
                            settingsWindow.pageStack.push(`${settingsWindow.settingsPath}/CustomCommandsSettings.qml`,
                                                               {m_customCommandsModel: root.m_customCommandsModel})
                        }
                    },
                    Kirigami.Action {
                        text: i18nc("@action", "Debug")
                        icon.name: "help-about"
                        onTriggered: {
                            root.activePage = SettingsWindow.Page.Debug
                            settingsWindow.pageStack.removePage(1)
                            settingsWindow.pageStack.push(`${settingsWindow.settingsPath}/DebugSettings.qml`,
                                                               {m_mpv: root.m_mpv})
                        }
                    },
                    Kirigami.Action {
                        text: i18nc("@action", "About")
                        icon.name: "help-about"
                        onTriggered: {
                            root.activePage = SettingsWindow.Page.About
                            settingsWindow.pageStack.removePage(1)
                            settingsWindow.pageStack.push(aboutPage)
                        }
                    }
                ]

                anchors.fill: parent
                model: pages
                delegate: ItemDelegate {
                    id: delegate

                    required property int index
                    required property Action modelData

                    width: settingsPagesList.width
                    action: delegate.modelData
                    highlighted: delegate.index === ListView.view.currentIndex
                }

                onCurrentIndexChanged: {
                    settingsPagesList.pages[root.activePage].trigger()
                }
            }

            Component {
                id: aboutPage

                Kirigami.AboutPage {
                    aboutData: KCoreAddons.AboutData
                }
            }
        }

        onClosing: root.active = false

        Component.onCompleted: pageStack.columnView.columnWidth = Kirigami.Units.gridUnit * 15

        Shortcut {
            sequence: "Esc"
            enabled: HarunaApp.actionsEnabled
            onActivated: settingsWindow.close()
        }
    }
}
