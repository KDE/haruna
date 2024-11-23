/*
 * SPDX-FileCopyrightText: 2022 Kartikey Subramanium <kartikey@tutanota.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.haruna

ToolButton {
    id: root

    required property MpvVideo m_mpv
    required property RecentFilesModel m_recentFilesModel

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
        active: menuBarLoader.state === "hidden"
        sourceComponent: Menu {
            id: menu

            y: root.position === HamburgerMenu.Position.Header
               ? root.height + Kirigami.Units.largeSpacing
               : -height - Kirigami.Units.largeSpacing
            visible:  root.isOpen
            closePolicy: Popup.CloseOnReleaseOutsideParent
            onClosed: root.isOpen = menu.visible
            modal: true

            MenuItem {
                action: appActions.openFileAction
                visible: root.position === HamburgerMenu.Position.Footer
            }
            MenuItem {
                action: appActions.openUrlAction
                visible: root.position === HamburgerMenu.Position.Footer
            }

            Menu {
                title: i18nc("@action:inmenu", "Recent Files")

                Repeater {
                    model: recentFilesModel
                    delegate: MenuItem {
                        id: delegate

                        required property string name
                        required property string path

                        text: delegate.name
                        onClicked: window.openFile(delegate.path)
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
                model: mpv.audioTracksModel
                enabled: mpv.audioTracksModel.rowCount() > 0

            }

            MenuSeparator {}

            MenuItem { action: appActions.toggleFullscreenAction }
            MenuItem { action: appActions.toggleDeinterlacingAction }
            MenuItem { action: appActions.screenshotAction }

            MenuSeparator {}

            MenuItem {
                action: appActions.configureAction
                visible: root.position === HamburgerMenu.Position.Footer
            }
            MenuItem { action: appActions.configureShortcutsAction }
            MenuItem { action: appActions.aboutHarunaAction }

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
                SettingsMenu {}
                HelpMenu {}
            }
        }
    }

}
