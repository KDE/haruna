/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.13
import QtQuick.Controls 2.13
import QtQuick.Layouts 1.13
import QtQml 2.13
import org.kde.kirigami 2.11 as Kirigami

import "Menus"

ToolBar {
    id: root

    property var audioTracks
    property var subtitleTracks

    position: ToolBar.Header
    visible: settings.get("View", "HeaderVisible")

    RowLayout {
        id: headerRow

        width: parent.width

        RowLayout {
            id: headerRowLeft

            Layout.alignment: Qt.AlignLeft

            ToolButton {
                action: actions.configureAction
            }
            ToolButton {
                action: actions.openAction
            }

            ToolButton {
                action: actions.openUrlAction
                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.MiddleButton
                    onClicked: {
                        openUrlTextField.clear()
                        openUrlTextField.paste()
                        window.openFile(openUrlTextField.text, true, false)
                    }
                }
            }

            ToolSeparator {
                id: toolSeparator

                padding: vertical ? 10 : 2
                topPadding: vertical ? 2 : 10
                bottomPadding: vertical ? 2 : 10

                contentItem: Rectangle {
                    implicitWidth: toolSeparator.vertical ? 1 : 24
                    implicitHeight: toolSeparator.vertical ? 24 : 1
                    color: Kirigami.Theme.textColor
                }
            }

            ToolButton {
                icon.name: "media-view-subtitles-symbolic"
                text: qsTr("Subtitles")

                onClicked: {
                    if (subtitleMenu.visible) {
                        subtitleMenu.close()
                    } else {
                        subtitleMenu.open()
                    }
                }

                Menu {
                    id: subtitleMenu

                    y: parent.height
                    onOpened: primaryMenuItems.model = mpv.subtitleTracksModel()

                    Menu {
                        id: secondarySubtitleMenu

                        title: qsTr("Secondary Subtitle")
                        onOpened: secondaryMenuItems.model = mpv.subtitleTracksModel()

                        TrackMenuItems {
                            id: secondaryMenuItems

                            menu: secondarySubtitleMenu
                            isFirst: false
                            onSubtitleChanged: function(id, index) {
                                mpv.setSecondarySubtitle(id)
                                mpv.subtitleTracksModel().updateSecondTrack(index)
                            }
                        }
                    }

                    MenuSeparator {}

                    MenuItem {
                        text: qsTr("Primary Subtitle")
                        hoverEnabled: false
                    }

                    TrackMenuItems {
                        id: primaryMenuItems
                        menu: subtitleMenu
                        isFirst: true

                        onSubtitleChanged: function(id, index) {
                            mpv.setSubtitle(id)
                            mpv.subtitleTracksModel().updateFirstTrack(index)
                        }
                    }
                }
            }

            ToolButton {
                icon.name: "audio-volume-high"
                text: qsTr("Audio")

                onClicked: {
                    if (audioMenu.visible) {
                        audioMenu.close()
                    } else {
                        audioMenu.open()
                    }

                    audioMenuInstantiator.model = mpv.audioTracksModel()
                }

                Menu {
                    id: audioMenu

                    y: parent.height

                    Instantiator {
                        id: audioMenuInstantiator

                        model: 0
                        onObjectAdded: (index, object) => audioMenu.insertItem( index, object )
                        onObjectRemoved: audioMenu.removeItem( object )
                        delegate: MenuItem {
                            id: audioMenuItem

                            required property var model

                            checkable: true
                            checked: model.isFirstTrack
                            text: model.text
                            onTriggered: {
                                mpv.setAudio(model.id)
                                mpv.audioTracksModel().updateFirstTrack(model.index)
                            }
                        }
                    }
                }
            }
        }

        RowLayout {
            id: headerRowRight

            Layout.alignment: Qt.AlignRight

            ToolButton {
                action: actions.quitApplicationAction
            }
        }
    }
}
