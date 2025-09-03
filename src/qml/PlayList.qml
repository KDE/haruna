/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.13
import QtQuick.Controls 2.13
import QtQuick.Window 2.13
import QtQuick.Layouts 1.13
import Qt5Compat.GraphicalEffects

Rectangle {
    id: root

    property alias tableView: tableView
    property bool canToggleWithMouse: settings.get("Playlist", "CanToogleWithMouse")
    property string position: settings.get("Playlist", "Position")
    property int rowHeight: settings.get("Playlist", "RowHeight")
    property int rowSpacing: settings.get("Playlist", "RowSpacing")
    property int bigFont: settings.get("Playlist", "BigFontFullscreen")

    height: mpv.height
    width: (parent.width * 0.33) < 550 ? 550 : parent.width * 0.33
    x: position === "right" ? parent.width : -width
    y: 0
    state: "hidden"

    onWidthChanged: {
        tableView.columnWidthProvider = (column) => tableView.columnWidths[column]
    }

    TableView {
        id: tableView

        property var columnWidths: [parent.width * 0.8, parent.width * 0.2]

        anchors.fill: parent
        boundsBehavior: Flickable.StopAtBounds
        columnSpacing: 1
        columnWidthProvider: (column) => columnWidths[column]
        delegate: PlayListItem {}
        rowSpacing: root.rowSpacing
        model: playListModel
        z: 20
        ScrollBar.vertical: ScrollBar { id: scrollBar }
    }

    MouseArea {
        anchors.fill: parent
        onWheel: wheel.accepted = true
    }

    ShaderEffectSource {
        id: shaderEffect
        anchors.fill: parent
        sourceItem: mpv
        sourceRect: position === "right"
                    ? Qt.rect(mpv.width - root.width, mpv.y, root.width, root.height)
                    : Qt.rect(0, 0, root.width, root.height)
    }

    FastBlur {
        anchors.fill: shaderEffect
        radius: 100
        source: shaderEffect
        z: 10
    }

    states: [
        State {
            name: "hidden"
            PropertyChanges { target: root; x: position === "right" ? parent.width : -width }
            PropertyChanges { target: root; visible: false }
        },
        State {
            name : "visible"
            PropertyChanges { target: root; x: position === "right" ? parent.width - root.width : mpv.x }
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
                    duration: 120
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
                    duration: 120
                    easing.type: Easing.OutQuad
                }
            }
        }
    ]
}
