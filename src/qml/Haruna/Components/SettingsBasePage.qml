/*
 * SPDX-FileCopyrightText: 2021 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQml 2.12

import org.kde.kirigami 2.11 as Kirigami
import org.kde.haruna 1.0

Kirigami.ScrollablePage {
    id: root

    property bool hasHelp: false
    property string helpFile: ""
    property string docPage: ""

    actions {
        contextualActions: [
            Kirigami.Action {
                text: i18nc("@action:intoolbar", "Help")
                iconName: "system-help"
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

}
