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
    id: hamburgerMenu

    icon.name: "application-menu"
    focusPolicy: Qt.NoFocus

    onReleased: {
        hamburgerMenuItem.visible = !hamburgerMenuItem.visible
    }

    Menu {
        id: hamburgerMenuItem

        y: parent.height
        closePolicy: Popup.CloseOnReleaseOutsideParent

        modal: true

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