/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import org.kde.kirigami 2.11 as Kirigami

Kirigami.Page
{
    padding: 0
    leftPadding: 0
    topPadding: 0
    rightPadding: 0
    bottomPadding: 0

    Component.onCompleted: applicationWindow().pageStack.columnView.columnWidth = 250


    footer: ToolBar {
        RowLayout {
            anchors.fill: parent

            ToolButton {
                text: i18n("Configure shortcuts")
                icon.name: "configure-shortcuts"
                onClicked: appActions.configureShortcutsAction.trigger()
                Layout.fillWidth: true
            }
        }
    }

    ListModel {
        id: settingsPagesModel
        ListElement {
            name: "General"
            iconName: "configure"
            page: "qrc:/General.qml"
        }
        ListElement {
            name: "Playback"
            iconName: "media-playback-start"
            page: "qrc:/Playback.qml"
        }
        ListElement {
            name: "Video"
            iconName: "video-x-generic"
            page: "qrc:/VideoSettings.qml"
        }
        ListElement {
            name: "Audio"
            iconName: "player-volume"
            page: "qrc:/Audio.qml"
        }
        ListElement {
            name: "Subtitles"
            iconName: "add-subtitle"
            page: "qrc:/Subtitles.qml"
        }
        ListElement {
            name: "Playlist"
            iconName: "view-media-playlist"
            page: "qrc:/Playlist.qml"
        }
        ListElement {
            name: "Mouse"
            iconName: "input-mouse"
            page: "qrc:/Mouse.qml"
        }
        ListElement {
            name: "Custom commands"
            iconName: "configure"
            page: "qrc:/CustomCommands.qml"
        }
    }

    ListView {
        id: settingsPagesList

        anchors.fill: parent
        model: settingsPagesModel
        delegate: Kirigami.BasicListItem {
            text: i18n(name)
            icon: iconName
            onClicked: applicationWindow().pageStack.push(model.page)
        }
    }
}
