/*
 * SPDX-FileCopyrightText: 2022 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import Qt.labs.platform as Labs

import org.kde.haruna

Labs.Menu {
    id: root

    title: i18nc("@title:menu", "&Help")

    Labs.MenuItem {
        icon.name: HarunaApp.actions.aboutHarunaAction.icon.name
        text: HarunaApp.actions.aboutHarunaAction.text
        onTriggered: HarunaApp.actions.aboutHarunaAction.trigger()
    }
    Labs.MenuItem {
        icon.name: HarunaApp.actions.reportBugAction.icon.name
        text: HarunaApp.actions.reportBugAction.text
        onTriggered: HarunaApp.actions.reportBugAction.trigger()
    }
    Labs.MenuItem {
        icon.name: HarunaApp.actions.sponsorAction.icon.name
        text: HarunaApp.actions.sponsorAction.text
        onTriggered: HarunaApp.actions.sponsorAction.trigger()
    }
}
