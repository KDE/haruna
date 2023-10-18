/*
 * SPDX-FileCopyrightText: 2022 Kartikey Subramanium <kartikey@tutanota.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import org.kde.kirigami 2.20 as Kirigami
import org.kde.haruna 1.0
import "../../Menus"

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
            model: mpv.subtitleTracksModel
            isPrimarySubtitleMenu: false
            title: i18nc("@title:menu", "Secondary Subtitle")
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

    SubtitleTracksMenu {
        id: primarySubtitlesMenu

        model: mpv.subtitleTracksModel
        isPrimarySubtitleMenu: true
        title: i18nc("@title:menu", "Primary Subtitle")

        Component.onCompleted: {
            if (root.position === HamburgerMenu.Position.Footer) {
                menu.insertMenu(4, primarySubtitlesMenu)
            }
        }
    }

    Menu {
        id: audioMenu

        title: i18nc("@title:menu", "&Audio Track")
        enabled: mpv.audioTracksModel.rowCount() > 0

        Repeater {
            id: audioMenuInstantiator
            model: mpv.audioTracksModel
            delegate: MenuItem {
                id: audioMenuItem
                checkable: true
                checked: model.id === mpv.audioId
                text: model.text
                onTriggered: mpv.audioId = model.id
            }
        }

        Component.onCompleted: {
            if (root.position === HamburgerMenu.Position.Footer) {
                menu.insertMenu(5, audioMenu)
            }
        }
    }
}
