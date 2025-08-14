/*
 * SPDX-FileCopyrightText: 2022 Kartikey Subramanium <kartikey@tutanota.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls

import org.kde.kirigami as Kirigami
import org.kde.haruna

ToolButton {
    id: root

    required property MpvVideo m_mpv
    required property RecentFilesModel m_recentFilesModel
    required property Loader m_settingsLoader
    required property MenuBarLoader m_menuBarLoader

    property int position: HamburgerMenu.Position.Header
    property bool isOpen: false

    enum Position {
        Header = 0,
        Footer
    }

    icon.name: "application-menu"
    focusPolicy: Qt.NoFocus

    onReleased: {
        root.isOpen = !root.isOpen
    }

    Loader {
        id: menuLoader

        asynchronous: true
        active: root.m_menuBarLoader.state === "hidden"
        sourceComponent: Menu {
            id: menu

            parent: root
            y: root.position === HamburgerMenu.Position.Header
               ? root.height + Kirigami.Units.largeSpacing
               : -height - Kirigami.Units.largeSpacing
            visible:  root.isOpen
            closePolicy: Popup.CloseOnReleaseOutside
            onClosed: root.isOpen = menu.visible
            modal: true

            MenuItem {
                action: HarunaApp.actions.openFileAction
                visible: root.position === HamburgerMenu.Position.Footer
            }
            MenuItem {
                action: HarunaApp.actions.openUrlAction
                visible: root.position === HamburgerMenu.Position.Footer
            }

            Menu {
                title: i18nc("@action:inmenu", "Recent Files")

                Repeater {
                    model: root.m_recentFilesModel
                    delegate: MenuItem {
                        id: delegate

                        required property string url
                        required property string filename
                        required property int openedFrom

                        text: openedFrom === RecentFilesModel.OpenedFrom.OpenAction
                              || openedFrom === RecentFilesModel.OpenedFrom.ExternalApp
                              ? `<strong>${delegate.filename}</strong>`
                              : delegate.filename

                        onClicked: {
                            const mainWindow = Window.window as Main
                            mainWindow.openFile(delegate.url, RecentFilesModel.OpenedFrom.Other)
                        }
                    }
                }
            }

            SubtitleTracksMenu {
                id: primarySubtitlesMenu

                m_mpv: root.m_mpv
                isPrimarySubtitleMenu: true
                title: i18nc("@title:menu", "Primary Subtitle")
            }

            SubtitleTracksMenu {
                m_mpv: root.m_mpv
                isPrimarySubtitleMenu: false
                title: i18nc("@title:menu", "Secondary Subtitle")
            }


            AudioTracksMenu {
                id: audioMenu

                m_mpv: root.m_mpv

                title: i18nc("@title:menu", "Audio Track")
                model: m_mpv.audioTracksModel
                enabled: m_mpv.audioTracksModel.rowCount > 0
            }

            MenuSeparator {}

            MenuItem { action: HarunaApp.actions.toggleFullscreenAction }
            MenuItem { action: HarunaApp.actions.toggleDeinterlacingAction }
            MenuItem { action: HarunaApp.actions.screenshotAction }

            MenuSeparator {}

            MenuItem {
                action: HarunaApp.actions.configureAction
                visible: root.position === HamburgerMenu.Position.Footer
            }
            MenuItem { action: HarunaApp.actions.configureShortcutsAction }
            MenuItem { action: HarunaApp.actions.aboutHarunaAction }

            MenuSeparator {}

            Menu {
                title: i18nc("@action:inmenu", "&More")

                FileMenu {
                    m_recentFilesModel: root.m_recentFilesModel
                }
                ViewMenu {}
                PlaybackMenu {}
                VideoMenu {}
                SubtitlesMenu {
                    m_mpv: root.m_mpv
                }
                AudioMenu {
                    m_mpv: root.m_mpv
                }
                SettingsMenu {
                    m_settingsLoader: root.m_settingsLoader
                }
                HelpMenu {}
            }
        }
    }

}
