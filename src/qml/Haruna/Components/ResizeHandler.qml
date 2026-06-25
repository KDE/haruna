/*
 * SPDX-FileCopyrightText: 2026 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

pragma ComponentBehavior: Bound
import QtQuick

Item {
    id: root

    property alias hovered: resizeHandlerMouseArea.containsMouse

    signal positionChanged(x: real)
    signal released()

    MouseArea {
        id: resizeHandlerMouseArea

        anchors.fill: parent
        acceptedButtons: Qt.LeftButton
        cursorShape: Qt.SizeHorCursor
        hoverEnabled: true

        drag {
            target: parent
            axis: Drag.XAxis
            threshold: 0
        }

        onPositionChanged: {
            if (!drag.active) {
                return
            }

            root.positionChanged(mouseX)
        }

        onReleased: {
            root.released()
        }
    }
}
