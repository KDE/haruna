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

    title: i18nc("@title:menu", "&Help")

    Labs.MenuItem {
        icon.name: appActions.aboutHarunaAction.icon.name
        text: appActions.aboutHarunaAction.text
        onTriggered: appActions.aboutHarunaAction.trigger()
    }
}
