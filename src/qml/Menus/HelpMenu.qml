/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick.Controls

import org.kde.haruna

Menu {
    id: root

    title: i18nc("@title:menu", "&Help")

    MenuItem { action: HarunaApp.actions.aboutHarunaAction }
    MenuItem { action: HarunaApp.actions.reportBugAction }
    MenuItem { action: HarunaApp.actions.sponsorAction }
}
