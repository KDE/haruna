/*
 * SPDX-FileCopyrightText: 2022 Kartikey Subramanium <kartikey@tutanota.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import org.kde.kirigami 2.11 as Kirigami
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
