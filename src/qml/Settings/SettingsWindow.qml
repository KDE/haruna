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
import org.kde.haruna

Kirigami.ApplicationWindow {
    id: root

    required property MpvVideo m_mpv
    required property ProxyActionsModel m_proxyActionsModel
    required property CustomCommandsModel m_customCommandsModel

    property string settingsPath: "qrc:/qt/qml/org/kde/haruna/qml/Settings"
    property int currentPage: SettingsWindow.Page.General
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

    width: Kirigami.Units.gridUnit * 45
    height: Kirigami.Units.gridUnit * 35
    title: i18nc("@title:window", "Settings")
    pageStack.initialPage: Kirigami.ScrollablePage {
        globalToolBarStyle: Kirigami.ApplicationHeaderStyle.None
        bottomPadding: 0
        leftPadding: 0
        rightPadding: 0
        topPadding: 0
        Kirigami.Theme.colorSet: Kirigami.Theme.View

        ListView {
            id: settingsPagesList

            currentIndex: root.currentPage
            Component.onCompleted: {
                pages[root.currentPage].trigger()
                currentIndex = root.currentPage
            }

            property list<Kirigami.Action> pages: [
                Kirigami.Action {
                    text: i18nc("@action", "General")
                    icon.name: "configure"
                    onTriggered: {
                        root.pageStack.removePage(1)
                        root.pageStack.push(`${root.settingsPath}/GeneralSettings.qml`)
                    }
                },
                Kirigami.Action {
                    text: i18nc("@action", "Playback")
                    icon.name: "media-playback-start"
                    onTriggered: {
                        root.pageStack.removePage(1)
                        root.pageStack.push(`${root.settingsPath}/PlaybackSettings.qml`)
                    }
                },
                Kirigami.Action {
                    text: i18nc("@action", "Video")
                    icon.name: "video-x-generic"
                    onTriggered: {
                        root.pageStack.removePage(1)
                        root.pageStack.push(`${root.settingsPath}/VideoSettings.qml`,
                                                           {m_mpv: root.m_mpv})
                    }
                },
                Kirigami.Action {
                    text: i18nc("@action", "Audio")
                    icon.name: "player-volume"
                    onTriggered: {
                        root.pageStack.removePage(1)
                        root.pageStack.push(`${root.settingsPath}/AudioSettings.qml`)
                    }
                },
                Kirigami.Action {
                    text: i18nc("@action", "Subtitles")
                    icon.name: "add-subtitle"
                    onTriggered: {
                        root.pageStack.removePage(1)
                        root.pageStack.push(`${root.settingsPath}/SubtitlesSettings.qml`)
                    }
                },
                Kirigami.Action {
                    text: i18nc("@action", "Playlist")
                    icon.name: "view-media-playlist"
                    onTriggered: {
                        root.pageStack.removePage(1)
                        root.pageStack.push(`${root.settingsPath}/PlaylistSettings.qml`)
                    }
                },
                Kirigami.Action {
                    text: i18nc("@action", "Mouse")
                    icon.name: "input-mouse"
                    onTriggered: {
                        root.pageStack.removePage(1)
                        root.pageStack.push(`${root.settingsPath}/MouseSettings.qml`)
                    }
                },
                Kirigami.Action {
                    text: i18nc("@action", "Shortcuts")
                    icon.name: "configure-shortcuts"
                    onTriggered: {
                        root.pageStack.removePage(1)
                        root.pageStack.push(`${root.settingsPath}/ShortcutsSettings.qml`,
                                                           {m_proxyActionsModel: root.m_proxyActionsModel})
                    }
                },
                Kirigami.Action {
                    text: i18nc("@action", "Custom commands")
                    icon.name: "configure"
                    onTriggered: {
                        root.pageStack.removePage(1)
                        root.pageStack.push(`${root.settingsPath}/CustomCommandsSettings.qml`,
                                                           {m_customCommandsModel: root.m_customCommandsModel})
                    }
                },
                Kirigami.Action {
                    text: i18nc("@action", "Debug")
                    icon.name: "help-about"
                    onTriggered: {
                        root.pageStack.removePage(1)
                        root.pageStack.push(`${root.settingsPath}/DebugSettings.qml`,
                                                           {m_mpv: root.m_mpv})
                    }
                },
                Kirigami.Action {
                    text: i18nc("@action", "About")
                    icon.name: "help-about"
                    onTriggered: {
                        root.pageStack.removePage(1)
                        root.pageStack.push(aboutPage)
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
                onClicked: {
                    ListView.view.currentIndex = delegate.index
                }
            }
        }

        Component {
            id: aboutPage

            Kirigami.AboutPage {
                aboutData: harunaAboutData
            }
        }
    }


    onCurrentPageChanged: {
        settingsPagesList.pages[root.currentPage].trigger()
        settingsPagesList.currentIndex = root.currentPage
    }

    Component.onCompleted: pageStack.columnView.columnWidth = Kirigami.Units.gridUnit * 15

    Shortcut {
        sequence: "Esc"
        onActivated: root.close()
    }
}
