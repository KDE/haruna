/*
 * SPDX-FileCopyrightText: 2021 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQml

import org.kde.kirigami as Kirigami
import org.kde.haruna

Kirigami.ScrollablePage {
    id: root

    property bool hasHelp: false
    property string helpFile: ""

    signal save()

    Component.onDestruction: save()

    actions: [
        Kirigami.Action {
            text: i18nc("@action:inmenu", "Help")
            icon.name: "system-help"
            visible: root.hasHelp
            onTriggered: root.hasHelp ? helpWindow.show() : undefined
        }
    ]

}
