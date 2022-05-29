/*
 * SPDX-FileCopyrightText: 2022 Kartikey Subramanium <kartikey@tutanota.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import org.kde.kirigami 2.11 as Kirigami
import org.kde.haruna 1.0
import "Menus"

ToolButton {
    id: root

    property alias menu: menu
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
            action: actions.openAction
            visible: root.position === HamburgerMenu.Position.Footer
        }
        MenuItem {
            action: actions.openUrlAction
            visible: root.position === HamburgerMenu.Position.Footer
        }

        Menu {
            title: i18n("Recent Files")

            Repeater {
                model: recentFilesModel
                delegate: MenuItem {
                    text: model.name
                    onClicked: window.openFile(model.path, true, PlaylistSettings.loadSiblings)
                }
            }
        }

        MenuSeparator {}

        MenuItem { action: actions.toggleFullscreenAction }
        MenuItem { action: actions.toggleDeinterlacingAction }
        MenuItem { action: actions.screenshotAction }

        MenuSeparator {}

        MenuItem {
            action: actions.configureAction
            visible: root.position === HamburgerMenu.Position.Footer
        }
        MenuItem { action: actions.configureShortcutsAction }
        MenuItem { action: actions.aboutHarunaAction }

        MenuSeparator {}

        Menu {
            title: i18n("&More")

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
