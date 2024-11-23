/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick.Controls

Menu {
    id: root

    title: i18nc("@title:menu", "&View")

    MenuItem { action: appActions.toggleFullscreenAction }

    MenuItem {
        action: appActions.toggleMenuBarAction
        text: menuBar.visible ? i18nc("@action:inmenu", "Hide Menubar") : i18nc("@action:inmenu", "Show Menubar")
    }
    MenuItem {
        action: appActions.toggleHeaderAction
        text: header.visible ? i18nc("@action:inmenu", "Hide Toolbar") : i18nc("@action:inmenu", "Show Toolbar")
    }
}
