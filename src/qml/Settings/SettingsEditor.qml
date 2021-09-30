/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.10
import QtQuick.Window 2.1
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtGraphicalEffects 1.12

import org.kde.kirigami 2.11 as Kirigami
import org.kde.haruna 1.0 as Haruna

Kirigami.ApplicationWindow {
    id: root

    property int page: SettingsEditor.Page.General
    enum Page {
        General = 0,
        Playback,
        Video,
        Audio,
        Subtitles,
        Playlist,
        Mouse,
        CusomCommands,
        About
    }

    width: Kirigami.Units.gridUnit * 45
    height: Kirigami.Units.gridUnit * 35
    title: i18n("Settings")
    visible: false
    pageStack.initialPage: Kirigami.ScrollablePage {
        bottomPadding: 0
        leftPadding: 0
        rightPadding: 0
        topPadding: 0
        Kirigami.Theme.colorSet: Kirigami.Theme.View

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

            Component.onCompleted: {
                pages[root.page].trigger()
                currentIndex = root.page
            }

            property list<Kirigami.Action> pages: [
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
                },
                Kirigami.Action {
                    text: i18n("About")
                    icon.name: "help-about"
                    onTriggered: {
                        applicationWindow().pageStack.removePage(1)
                        applicationWindow().pageStack.push(aboutPage)
                    }
                }
            ]

            anchors.fill: parent
            model: pages
            delegate: Kirigami.BasicListItem {
                action: modelData
            }
        }

        Component {
            id: aboutPage

            Kirigami.AboutPage {
                aboutData: harunaAboutData
            }
        }
    }

    onVisibleChanged: {
        if (visible) {
            settingsPagesList.pages[root.page].trigger()
            settingsPagesList.currentIndex = root.page
        }
    }

    Component.onCompleted: pageStack.columnView.columnWidth = Kirigami.Units.gridUnit * 15

    Window {
        id: helpWindow

        width: Kirigami.Units.gridUnit * 40
        height: Kirigami.Units.gridUnit * 35
        title: i18n("Help")
        color: Kirigami.Theme.backgroundColor
        onVisibleChanged: info.text = app.getFileContent(applicationWindow().pageStack.currentItem.helpFile)

        Flickable {
            id: scrollView

            property int scrollStepSize: 100

            anchors.fill: parent
            contentHeight: info.height

            ScrollBar.vertical: ScrollBar {
                id: scrollbar
                policy: ScrollBar.AlwaysOn
                stepSize: scrollView.scrollStepSize/scrollView.contentHeight
            }

            MouseArea {
                anchors.fill: parent
                onWheel: {
                    if (wheel.angleDelta.y > 0) {
                        scrollbar.decrease()
                    } else {
                        scrollbar.increase()
                    }
                }
            }

            TextArea {
                id: info

                background: Rectangle {
                    color: "transparent"
                    border.color: "transparent"
                }
                width: parent.width
                color: Kirigami.Theme.textColor
                readOnly: true
                textFormat: Text.RichText
                wrapMode: Text.WordWrap
                selectByMouse: true
                rightPadding: scrollbar.width
                onLinkActivated: Qt.openUrlExternally(link)
                onHoveredLinkChanged: hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
            }
        }
    }
}
