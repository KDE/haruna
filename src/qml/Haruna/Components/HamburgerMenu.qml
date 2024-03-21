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

    property int position: HamburgerMenu.Position.Header

    enum Position {
        Header = 0,
        Footer
    }

    icon.name: "application-menu"
    focusPolicy: Qt.NoFocus

    onReleased: {
        menu.visible = !menu.visible
    }

    Menu {
        id: menu

        y: root.position === HamburgerMenu.Position.Header
           ? parent.height + Kirigami.Units.smallSpacing
           : -height - Kirigami.Units.smallSpacing
        closePolicy: Popup.CloseOnReleaseOutsideParent

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
                    text: model.name
                    onClicked: window.openFile(model.path)
                }
            }
        }

        SubtitleTracksMenu {
            id: primarySubtitlesMenu

            model: mpv.subtitleTracksModel
            isPrimarySubtitleMenu: true
            title: i18nc("@title:menu", "Primary Subtitle")
        }

        SubtitleTracksMenu {
            model: mpv.subtitleTracksModel
            isPrimarySubtitleMenu: false
            title: i18nc("@title:menu", "Secondary Subtitle")
        }


        AudioTracksMenu {
            id: audioMenu

            y: parent.height
            title: i18nc("@title:menu", "Audio Track")
            model: mpv.audioTracksModel
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

            FileMenu {}
            ViewMenu {}
            PlaybackMenu {}
            VideoMenu {}
            SubtitlesMenu {}
            AudioMenu {}
            SettingsMenu {}
            HelpMenu {}
        }
    }

}
