/*
 * SPDX-FileCopyrightText: 2026 George Florea Bănuș <georgefb899@gmail.com>
 * SPDX-FileCopyrightText: 2026 Muhammet Sadık Uğursoy <sadikugursoy@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls

import org.kde.kirigami as Kirigami
import org.kde.haruna

Page {
    id: root

    required property MpvVideo m_mpv
    required property real fsScale
    required property int mainWindowWidth

    property bool isSmallWindowSize: mainWindowWidth < 600
    property int buttonSize: Kirigami.Units.iconSizes.small
    property int edge: Qt.RightEdge
    property real customWidth: 260

    property alias pageContent: pageContent.data

    readonly property alias pageEdgeBorder: pageEdgeBorder

    signal saveWidth()
    signal resize(int delta)

    x: root.edge === Qt.RightEdge ? root.mainWindowWidth : -root.width
    y: 0
    padding: 0
    state: "hidden"
    width: customWidth

    Rectangle {
        anchors.fill: parent
        color: Kirigami.Theme.backgroundColor

        // Playlist and Transcript override this Item
        Item {
            id: pageContent
            anchors.fill: parent
        }

        Rectangle {
            id: pageEdgeBorder

            x: root.edge === Qt.RightEdge ? 0 : parent.width - width
            y: -root.implicitHeaderHeight
            width: 1
            height: root.height
            color: Kirigami.Theme.backgroundColor

            ResizeHandler {
                id: resizeHandlerTall

                anchors.horizontalCenter: parent.horizontalCenter
                width: 8
                height: parent.height

                onPositionChanged: function (pX) {
                    root.resize(pX)
                }

                onReleased: {
                    root.saveWidth()
                }
            }

            Rectangle {
                id: dragHandle

                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                width: 5
                height: 50
                color: (resizeHandlerTall.hovered || resizeHandlerCentral.hovered) ? Kirigami.Theme.highlightColor : Kirigami.Theme.alternateBackgroundColor
                radius: Kirigami.Units.cornerRadius
                border {
                    width: 1
                    color: Kirigami.Theme.backgroundColor
                }

                ResizeHandler {
                    id: resizeHandlerCentral

                    anchors.horizontalCenter: parent.horizontalCenter
                    width: 18
                    height: parent.height

                    onPositionChanged: function (pX) {
                        root.resize(pX)
                    }

                    onReleased: {
                        root.saveWidth()
                    }
                }
            }
        }
    }

    states: [
        State {
            name: "hidden"

            PropertyChanges {
                root.x: root.edge === Qt.RightEdge
                        ? root.mainWindowWidth
                        : -width
                root.visible: false
            }
        },
        State {
            name : "visible"

            PropertyChanges {
                root.x: root.edge === Qt.RightEdge
                        ? root.mainWindowWidth - root.width + 1
                        : 0
                root.visible: true
            }
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
                    duration: Kirigami.Units.longDuration
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
                    duration: Kirigami.Units.longDuration
                    easing.type: Easing.OutQuad
                }
            }
        }
    ]
}
