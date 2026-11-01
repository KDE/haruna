/*
 * SPDX-FileCopyrightText: 2021 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick

import org.kde.kirigami as Kirigami

Kirigami.ScrollablePage {
    id: root

    signal save()

    globalToolBarStyle: Kirigami.ApplicationHeaderStyle.None

    Component.onDestruction: save()
}
