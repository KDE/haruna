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
    property string docPage: ""

    signal save()

    Component.onDestruction: save()

    actions: [
        Kirigami.Action {
            text: i18nc("@action:intoolbar", "Help")
            icon.name: "system-help"
            Kirigami.Action {
                text: i18nc("@action:inmenu", "Haruna help window (english only)")
                enabled: root.hasHelp
                onTriggered: root.hasHelp ? helpWindow.show() : undefined
            }
            Kirigami.Action {
                text: i18nc("@action:inmenu", "Haruna handbook")
                enabled: root.docPage !== ""
                onTriggered: enabled ? app.openDocs(root.docPage) : undefined
            }
        }
    ]

}
