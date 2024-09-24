/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQml 2.12
import org.kde.kirigami 2.11 as Kirigami
import AppSettings 1.0

import "Menus"

ToolBar {
    id: root

    property var audioTracks
    property var subtitleTracks

    position: ToolBar.Header
    visible: !window.isFullScreen() && AppSettings.viewIsHeaderVisible

    RowLayout {
        id: headerRow

        width: parent.width

        RowLayout {
            id: headerRowLeft

            Layout.alignment: Qt.AlignLeft

            ToolButton {
                action: actions.configureAction
                checkable: true
                checked: settingsEditor.state === "visible"
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
                padding: vertical ? 10 : 2
                topPadding: vertical ? 2 : 10
                bottomPadding: vertical ? 2 : 10

                contentItem: Rectangle {
                    implicitWidth: parent.vertical ? 1 : 24
                    implicitHeight: parent.vertical ? 24 : 1
                    color: Kirigami.Theme.textColor
                }
            }

            ToolButton {
                id: subtitleMenuButton

                property var model: 0

                text: qsTr("Subtitles")
                icon.name: "media-view-subtitles-symbolic"

                onClicked: {
                    if (subtitleMenuButton.model === 0) {
                        subtitleMenuButton.model = mpv.subtitleTracksModel()
                    }

                    subtitleMenu.visible = !subtitleMenu.visible
                }

                Menu {
                    id: subtitleMenu

                    y: parent.height

                    Menu {
                        id: secondarySubtitleMenu

                        title: qsTr("Secondary Subtitle")

                        Instantiator {
                            id: secondarySubtitleMenuInstantiator
                            model: subtitleMenuButton.model
                            onObjectAdded: function(index, object) {
                                secondarySubtitleMenu.insertItem( index, object )
                            }
                            onObjectRemoved: function(index, object) {
                                secondarySubtitleMenu.removeItem( object )
                            }
                            delegate: MenuItem {
                                enabled: model.id !== mpv.subtitleId || model.id === 0
                                checkable: true
                                checked: model.id === mpv.secondarySubtitleId
                                text: model.text
                                onTriggered: mpv.secondarySubtitleId = model.id
                            }
                        }
                    }

                    MenuSeparator {}

                    MenuItem {
                        text: qsTr("Primary Subtitle")
                        hoverEnabled: false
                    }

                    Instantiator {
                        id: primarySubtitleMenuInstantiator
                        model: subtitleMenuButton.model
                        onObjectAdded: function(index, object) {
                            subtitleMenu.insertItem( index, object )
                        }
                        onObjectRemoved: function(index, object) {
                            subtitleMenu.removeItem( object )
                        }
                        delegate: MenuItem {
                            enabled: model.id !== mpv.secondarySubtitleId || model.id === 0
                            checkable: true
                            checked: model.id === mpv.subtitleId
                            text: model.text
                            onTriggered: mpv.subtitleId = model.id
                        }
                    }
                }
            }

            ToolButton {
                text: qsTr("Audio")
                icon.name: "audio-volume-high"

                onClicked: {
                    if (audioMenuInstantiator.model === 0) {
                        audioMenuInstantiator.model = mpv.audioTracksModel()
                    }
                    audioMenu.visible = !audioMenu.visible
                }

                Menu {
                    id: audioMenu

                    y: parent.height

                    Instantiator {
                        id: audioMenuInstantiator

                        model: 0
                        onObjectAdded: function(index, object) {
                            audioMenu.insertItem( index, object )
                        }
                        onObjectRemoved: function(index, object) {
                            audioMenu.removeItem( object )
                        }
                        delegate: MenuItem {
                            id: audioMenuItem
                            checkable: true
                            checked: model.id === mpv.audioId
                            text: model.text
                            onTriggered: mpv.audioId = model.id
                        }
                    }
                }
            }
        }

        RowLayout {
            id: headerRowRight

            Layout.alignment: Qt.AlignRight

            ToolButton {
                // using `action: actions.quitApplicationAction` breaks the action
                // doens't work on the first try in certain circumstances
                text: actions.quitApplicationAction.text
                icon: actions.quitApplicationAction.icon
                onClicked: actions.quitApplicationAction.trigger()
            }
        }
    }
}
