/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls

import org.kde.kirigami as Kirigami
import org.kde.haruna
import org.kde.haruna.settings

TabButton {
    id: root

    required property int index
    required property string name
    required property bool isVisible
    required property bool isActive
    required property MpvVideo m_mpv

    property bool aboutToBeRemoved: false
    property int dragCenter: 0

    // Normally, 'dragHandler.activeTranslation.x < 0 ' check would suffice to detect a drag direction, but
    // that variable gets no updates after a tab move. So, we offset it by calculating the new tab center manually
    property bool leftDrag: dragHandler.activeTranslation.x < dragCenter
    property int maximumTabWidth: 150

    implicitWidth: calculateWidth()
    text: index === 0
              ? i18nc("@label:title name of default playlist tab", "Default")
              : name


    TextMetrics {
        id: metrics
        font: root.font
        text: root.name
    }

    Drag.active: dragHandler.active
    Drag.hotSpot.x: calculateHotSpot()

    Item {
        id: leftItem

        anchors {
            left: root.left
            verticalCenter: root.verticalCenter
            leftMargin: leftItem.state !== "Drag" ? Kirigami.Units.largeSpacing : Kirigami.Units.mediumSpacing
        }

        width: state !== "Drag" ? Kirigami.Units.iconSizes.small : Kirigami.Units.iconSizes.smallMedium
        height: state !== "Drag" ? Kirigami.Units.iconSizes.small : Kirigami.Units.iconSizes.smallMedium

        states: [
            State {
                name: "Play"; when: root.isActive && (root.index === 0 || !leftItemHover.hovered)
            },
            State {
                name: "Drag"; when: root.index !== 0
            },
            State {
                name: "Empty"
            }

        ]

        Kirigami.Icon {
            id: dragIcon
            z: 4

            anchors.fill: parent

            source: {
                switch(leftItem.state){
                case "Empty":
                    return ""
                case "Play":
                    return "media-playback-start"
                case "Drag":
                    return "handle-sort"
                }
            }

            width: leftItem.state !== "Drag" ? Kirigami.Units.iconSizes.small : Kirigami.Units.iconSizes.smallMedium
            height: leftItem.state !== "Drag" ? Kirigami.Units.iconSizes.small : Kirigami.Units.iconSizes.smallMedium
            transform: leftItem.state !== "Drag" ? [] : rotateIcon

            Rotation {
                id: rotateIcon
                origin.x: dragIcon.width * 0.5
                origin.y: dragIcon.height * 0.5
                angle: 90
            }
        }

        HoverHandler {
            id: leftItemHover
            cursorShape: root.index !== 0 ? Qt.OpenHandCursor : Qt.ArrowCursor
        }

        DragHandler {
            id: dragHandler

            acceptedButtons: Qt.LeftButton
            dragThreshold: 10
            cursorShape: active ? Qt.ClosedHandCursor : Qt.OpenHandCursor
            enabled: root.index !== 0

            target: root
            yAxis.enabled: false
            xAxis {
                enabled: true
                minimum: 0
                maximum: root.TabBar.tabBar?.contentWidth
            }

            onGrabChanged: function(transition, eventPoint) {
                switch(transition){
                case PointerDevice.GrabExclusive:
                case PointerDevice.GrabPassive:
                    root.z = 100
                    break
                case PointerDevice.CancelGrabExclusive:
                case PointerDevice.CancelGrabPassive:
                case PointerDevice.UngrabExclusive:
                case PointerDevice.UngrabPassive:
                    root.z = 0
                    if (root.index !== 0) {
                        var prevItem = root.TabBar.tabBar.itemAt(root.index - 1)
                        root.x = prevItem.x + prevItem.width
                    }
                    root.dragCenter = 0
                    break
                }
            }
        }
    }

    DropArea {
        id: dropArea
        anchors.fill: parent

        onEntered: function (drag) {
            const from = drag.source.index
            const to = root.index
            const leftDrag = drag.source.leftDrag

            if (to === 0) {
                return
            }

            // Depending on the current dragging direction, update the dragCenter so that
            // we can update dragging direction even if the user doesn't let go the drag.
            // This is necessary because we use dragHandler.activeTranslation.x to detect dragging
            // direction and it always references the initial drag position, even after a tab move occurs.
            // We want to re-check from the new position, after the tab move. dragCenter effectively updates
            // the dragHandler.activeTranslation.x
            if (leftDrag) {
                if (from < to) return
                drag.source.dragCenter -= root.width
            }
            if (!leftDrag) {
                if (from > to) return
                drag.source.dragCenter += root.width
            }

            root.TabBar.tabBar.movePlaylistItem(from, to)
            root.movePlaylist(from, to)
        }
    }

    Rectangle {
        id: rightItem
        visible: root.index !== 0

        anchors {
            right: root.right
            verticalCenter: root.verticalCenter
            rightMargin: Kirigami.Units.largeSpacing
        }

        width: Kirigami.Units.iconSizes.small
        height: Kirigami.Units.iconSizes.small
        radius: Kirigami.Units.iconSizes.small

        color: rightItemHover.hovered ? Kirigami.Theme.negativeTextColor : "transparent"

        Kirigami.Icon {
            id: closeIcon
            z: 4

            anchors.fill: parent
            source: "dialog-close"
            color: rightItemHover.hovered ? Kirigami.Theme.backgroundColor : Kirigami.Theme.textColor

            width: Kirigami.Units.iconSizes.small
            height: Kirigami.Units.iconSizes.small
        }

        HoverHandler {
            id: rightItemHover
        }

        TapHandler {
            id: rightItemTap
            dragThreshold: Kirigami.Units.iconSizes.small * 0.75

            onPressedChanged: {
                if (pressed) {
                    progressTimer.start()
                    progressTimer.heldAmount = 0.0
                }
                else {
                    progressTimer.stop()
                    progressTimer.heldAmount = 0.0
                }
            }
        }
    }

    Timer {
        id: progressTimer

        property real holdThreshold: 0.8 //seconds
        property real heldAmount: 0.0 //seconds
        property real progress: heldAmount / holdThreshold

        interval: 10
        running: false
        repeat: true

        onTriggered: {
            heldAmount += 0.01 //ms
            if (progress >= 1.0) {
                root.removePlaylist()
                stop()
            }
        }
    }

    Rectangle {
        id: removingProgressOverlay
        z: 0
        visible: progressTimer.progress > 0.0
        color: Qt.alpha(Kirigami.Theme.negativeTextColor, 0.5)
        anchors {
            left: root.left
            top: root.top
            bottom: root.bottom
        }

        width: Math.min(progressTimer.progress * root.width, root.width)
    }

    Item {
        anchors.fill: parent
        TapHandler {
            enabled: root.index !== 0
            acceptedButtons: Qt.MiddleButton
            acceptedModifiers: Qt.NoModifier
            grabPermissions: PointerHandler.TakeOverForbidden

            onPressedChanged: {
                if (pressed) {
                    progressTimer.start()
                    progressTimer.heldAmount = 0.0
                }
                else {
                    progressTimer.stop()
                    progressTimer.heldAmount = 0.0
                }
            }
        }

        TapHandler {
            acceptedButtons: Qt.RightButton
            acceptedModifiers: Qt.NoModifier
            grabPermissions: PointerHandler.TakeOverForbidden

            onTapped: {
                root.TabBar.tabBar.openContextMenu(root)
            }
        }
    }


    function movePlaylist(from, to) {
        root.m_mpv.playlists.movePlaylist(from, to)
    }

    function removePlaylist() {
        root.m_mpv.playlists.removePlaylist(root.index)
    }

    function calculateHotSpot() {
        let defaultSpot = dragIcon.width * 0.5 + dragIcon.x
        if (!dragHandler.active) {
            return defaultSpot
        }

        if (root.leftDrag) {
            if (root.index === 0) {
                return dragIcon.x
            }
            var item = root.TabBar.tabBar.itemAt(root.index - 1)
            return item.width - dragIcon.width
        }
        else {
            if (root.index === root.TabBar.tabBar.count - 1) {
                return dragIcon.width + dragIcon.x
            }
            var item = root.TabBar.tabBar.itemAt(root.index + 1)
            return -(item.width - root.width)
        }
        return defaultSpot
    }

    function calculateWidth() {
        let margins = leftItem.anchors.leftMargin + leftItem.width + rightItem.width + rightItem.anchors.rightMargin
        return Math.min( // cap by maximumTabWidth
                Math.min( // calculate text width and add handlerMargins+spacing VS. default implicit
                    metrics.tightBoundingRect.width + Kirigami.Units.mediumSpacing * 2,
                    implicitBackgroundWidth)
                + margins,
                maximumTabWidth)
    }
}
