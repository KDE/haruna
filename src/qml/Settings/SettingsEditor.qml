/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.10
import QtQuick.Window 2.1
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import Qt5Compat.GraphicalEffects
import org.kde.kirigami 2.11 as Kirigami

Pane {
    id: root

    signal skipChaptersChanged(bool checked)

    x: -width; y: 0; z: 50
    width: 600
    height: mpv.height
    leftPadding: 0
    rightPadding: 0
    state: "hidden"
    hoverEnabled: true

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Navigation {
            id: nav

            width: root.width * 0.3 - root.padding
            Layout.fillHeight: true
        }

        Rectangle {
            width: 1
            color: Kirigami.Theme.alternateBackgroundColor
            Layout.fillHeight: true
        }

        ColumnLayout {
            spacing: 0

            Flickable {
                id: settingsPage

                clip: true
                height: root.height
                contentHeight: root.height - root.topPadding - root.bottomPadding - settingsHelpButton.height
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.leftMargin: 10
                Layout.rightMargin: 5
                ScrollBar.vertical: ScrollBar { id: settingsPageScrollBar }

                Loader {
                    id: settingsPageLoader

                    anchors.leftMargin: settingsPageScrollBar.width
                    anchors.rightMargin: settingsPageScrollBar.width
                    sourceComponent: generalSettings
                }
            }

            Button {
                id: settingsHelpButton

                text: qsTr("Help!")
                icon.name: "system-help"
                visible: settingsPageLoader.item.hasHelp
                onClicked: settingsPageLoader.item.hasHelp ? helpWindow.show() : undefined
                Layout.alignment: Qt.AlignBottom
                Layout.leftMargin: 10
                Layout.rightMargin: 5
            }
        }
    }

    Component {
        id: generalSettings
        General {
            width: root.width * 0.7 - root.padding
        }
    }
    Component {
        id: videoSettings
        VideoSettings {
            width: root.width * 0.7 - root.padding
        }
    }
    Component {
        id: colorAdjustmentsSettings
        ColorAdjustments {
            width: root.width * 0.7 - root.padding
        }
    }
    Component {
        id: mouseSettings
        Mouse {
            width: root.width * 0.7 - root.padding
        }
    }
    Component {
        id: playlistSettings
        Playlist {
            width: root.width * 0.7 - root.padding
        }
    }
    Component {
        id: audioSettings
        Audio {
            width: root.width * 0.7 - root.padding
        }
    }
    Component {
        id: subtitlesSettings
        Subtitles {
            width: root.width * 0.7 - root.padding
        }
    }
    Component {
        id: playbackSettings
        Playback {
            width: root.width * 0.7 - root.padding
        }
    }

    states: [
        State {
            name: "hidden"
            PropertyChanges { target: root; x: -width }
            PropertyChanges { target: root; visible: false }
        },
        State {
            name : "visible"
            PropertyChanges { target: root; x: 0 }
            PropertyChanges { target: root; visible: true }
        }
    ]

    transitions: [
        Transition {
            from: "visible"
            to: "hidden"

            SequentialAnimation {
                NumberAnimation {
                    target: root
                    property: "x"
                    duration: 150
                    easing.type: Easing.InQuad
                }
                PropertyAction {
                    target: root
                    property: "visible"
                    value: false
                }
            }
        },
        Transition {
            from: "hidden"
            to: "visible"

            SequentialAnimation {
                PropertyAction {
                    target: root
                    property: "visible"
                    value: true
                }
                NumberAnimation {
                    target: root
                    property: "x"
                    duration: 150
                    easing.type: Easing.OutQuad
                }
            }
        }
    ]

    Window {
        id: helpWindow

        width: 700
        height: 600
        title: qsTr("Help")
        color: Kirigami.Theme.backgroundColor
        onVisibleChanged: info.text = app.getFileContent(settingsPageLoader.item.helpFile)

        Flickable {
            id: infoFlickable
            anchors.fill: parent
            contentHeight: info.height
            TextEdit {
                id: info

                width: parent.width
                rightPadding: scrollBar.width
                color: Kirigami.Theme.textColor
                readOnly: true
                textFormat: Text.RichText
                wrapMode: Text.WordWrap
                selectByMouse: true
                onLinkActivated: Qt.openUrlExternally(link)
                onHoveredLinkChanged: hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
                textMargin: scrollBar.width
            }
            ScrollBar.vertical: ScrollBar {
                id: scrollBar
            }
        }
    }
}
