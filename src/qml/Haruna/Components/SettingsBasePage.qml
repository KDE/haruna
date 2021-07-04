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
                text: i18n("Help!")
                iconName: "system-help"
                Kirigami.Action {
                    text: i18n("Haruna help window (english only)")
                    enabled: root.hasHelp
                    onTriggered: root.hasHelp ? helpWindow.show() : undefined
                }
                Kirigami.Action {
                    text: i18n("KHelpCenter")
                    enabled: root.docPage !== ""
                    onTriggered: enabled ? app.openDocs(root.docPage) : undefined
                }
            },
            Kirigami.Action {
                text: i18n("Open config ...")
                iconName: "folder"
                Kirigami.Action {
                    text: i18n("File")
                    onTriggered: Qt.openUrlExternally(app.configFilePath)
                }
                Kirigami.Action {
                    text: i18n("Folder")
                    onTriggered: Qt.openUrlExternally(app.configFolderPath)
                }
            }
        ]
    }

}
