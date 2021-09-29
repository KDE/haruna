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

    Component.onCompleted: applicationWindow().pageStack.columnView.columnWidth = Kirigami.Units.gridUnit * 15

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

    ListView {
        id: settingsPagesList

        property list<Kirigami.Action> actions: [
            Kirigami.Action {
                text: i18n("General")
                icon.name: "configure"
                onTriggered: {
                    applicationWindow().pageStack.removePage(1)
                    applicationWindow().pageStack.push("qrc:/Playback.qml")
                }
            },
            Kirigami.Action {
                text: i18n("Playback")
                icon.name: "media-playback-start"
                onTriggered: {
                    applicationWindow().pageStack.removePage(1)
                    applicationWindow().pageStack.push("qrc:/Playback.qml")
                }
            },
            Kirigami.Action {
                text: i18n("Video")
                icon.name: "video-x-generic"
                onTriggered: {
                    applicationWindow().pageStack.removePage(1)
                    applicationWindow().pageStack.push("qrc:/VideoSettings.qml")
                }
            },
            Kirigami.Action {
                text: i18n("Audio")
                icon.name: "player-volume"
                onTriggered: {
                    applicationWindow().pageStack.removePage(1)
                    applicationWindow().pageStack.push("qrc:/Audio.qml")
                }
            },
            Kirigami.Action {
                text: i18n("Subtitles")
                icon.name: "add-subtitle"
                onTriggered: {
                    applicationWindow().pageStack.removePage(1)
                    applicationWindow().pageStack.push("qrc:/Subtitles.qml")
                }
            },
            Kirigami.Action {
                text: i18n("Playlist")
                icon.name: "view-media-playlist"
                onTriggered: {
                    applicationWindow().pageStack.removePage(1)
                    applicationWindow().pageStack.push("qrc:/Playlist.qml")
                }
            },
            Kirigami.Action {
                text: i18n("Mouse")
                icon.name: "input-mouse"
                onTriggered: {
                    applicationWindow().pageStack.removePage(1)
                    applicationWindow().pageStack.push("qrc:/Mouse.qml")
                }
            },
            Kirigami.Action {
                text: i18n("Custom commands")
                icon.name: "configure"
                onTriggered: {
                    applicationWindow().pageStack.removePage(1)
                    applicationWindow().pageStack.push("qrc:/CustomCommands.qml")
                }
            }
        ]

        anchors.fill: parent
        model: actions
        delegate: Kirigami.BasicListItem {
            action: modelData
        }
    }
}
