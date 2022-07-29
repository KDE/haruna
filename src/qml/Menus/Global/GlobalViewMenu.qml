/*
 * SPDX-FileCopyrightText: 2022 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.12
import QtQuick.Controls 2.12
import Qt.labs.platform 1.1 as Labs

Labs.Menu {
    id: root

    title: i18n("&View")

    Labs.MenuItem {
        icon.name: appActions.toggleFullscreenAction.icon.name
        text: appActions.toggleFullscreenAction.text
        onTriggered: appActions.toggleFullscreenAction.trigger()
    }

    Labs.MenuItem {
        icon.name: appActions.toggleMenuBarAction.icon.name
        text: menuBar.visible ? i18n("Hide Menubar") : i18n("Show Menubar")
        onTriggered: appActions.toggleMenuBarAction.trigger()
    }

    Labs.MenuItem {
        icon.name: appActions.toggleHeaderAction.icon.name
        text: header.visible ? i18n("Hide Toolbar") : i18n("Show Toolbar")
        onTriggered: appActions.toggleHeaderAction.trigger()
    }
}
