/*
 * SPDX-FileCopyrightText: 2022 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls
import Qt.labs.platform as Labs

Labs.Menu {
    id: root

    title: i18nc("@title:menu", "&Help")

    Labs.MenuItem {
        icon.name: appActions.aboutHarunaAction.icon.name
        text: appActions.aboutHarunaAction.text
        onTriggered: appActions.aboutHarunaAction.trigger()
    }
}
