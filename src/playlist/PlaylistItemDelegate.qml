/*
 * SPDX-FileCopyrightText: 2025 Muhammet Sadık Uğursoy <sadikugursoy@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Shapes

import org.kde.kirigami as Kirigami
import org.kde.haruna
import org.kde.haruna.settings

Item {
    id: root

    required property int index
    required property string title
    required property string name
    required property string duration
    required property string path
    required property bool isLocal
    required property bool isPlaying
    required property bool isSelected
    required property string section
    required property MpvVideo m_mpv

    property alias contentItem: contentItem.data
    property alias dragRect: backgroundRect
    property string rowNumber: (index + 1).toString()
    property real alpha: PlaylistSettings.overlayVideo ? 0.6 : 1
    property real cachedHeight: height

    implicitWidth: ListView.view.width

    states: [
        State {
            name: "Normal";
        },
        State {
            name: "Highlighted"; when: root.isPlaying && !(hoverHandler.hovered || root.isSelected)
        },
        State {
            name: "Hovered"; when: hoverHandler.hovered && !root.isSelected
        },
        State {
            name: "Selected"; when: root.isSelected
        }
    ]

    SequentialAnimation {
        id: removeAnimation

        PropertyAction { target: root; property: "ListView.delayRemove"; value: true }

        ParallelAnimation {
            NumberAnimation { target: root; property: "height"; to: 0; duration: Kirigami.Units.shortDuration }
            NumberAnimation { target: root; property: "opacity"; to: 0; duration: Kirigami.Units.shortDuration }
        }

        PropertyAction { target: root; property: "ListView.delayRemove"; value: false }
    }

    ListView.onRemove: {
        root.cachedHeight = root.height
        removeAnimation.start()
    }

    ListView.onPooled: {
        root.opacity = 1.0
        root.height = root.cachedHeight
    }

    ListView.onReused: {
        root.opacity = 1.0
        root.height = root.cachedHeight
    }

    Rectangle {
        id: backgroundRect

        width: root.width
        height: root.height

        Kirigami.Theme.colorSet: Kirigami.Theme.View

        color: {
            if (root.state === "Highlighted") {
                return Qt.alpha(Kirigami.Theme.highlightColor, root.alpha)
            }
            if (root.state === "Hovered") {
                return Qt.alpha(Kirigami.Theme.hoverColor, root.alpha)
            }
            if (root.state === "Selected") {
                return Qt.lighter(Kirigami.Theme.highlightColor, 1.1)
            }
            return Qt.alpha(Kirigami.Theme.backgroundColor, root.alpha)
        }

        Behavior on color { ColorAnimation { duration: Kirigami.Units.shortDuration } }

        // Derived items must override this
        Item {
            id: contentItem
        }
    }

    DropArea {
        id: itemDropArea

        anchors.fill: parent
        keys: ["text/uri-list"]

        onDropped: function (drop) {
            if (!containsDrag) {
                return
            }
            root.m_mpv.visibleFilterProxyModel.addFilesAndFolders(drop.urls, PlaylistModel.Insert, root.index)
        }
    }

    Shape {
        visible: itemDropArea.containsDrag
        antialiasing: true
        z: 1
        ShapePath {
            id: shape

            property int arrowWidth: 6
            property int arrowHeight: 6
            property int centerThickness: 1
            property int listViewSpacing: root.ListView?.view.spacing
            strokeWidth: 0
            startX: 0
            startY: 0
            fillColor: Kirigami.Theme.highlightColor
            /* Order of PathLines:
              8\                  /5
              | \                / |
              |  7--------------6  |
              |  2--------------3  |
              | /                \ |
              1/                  \4
            */
            PathLine {
                x: 0;
                y: shape.arrowHeight
            }
            PathLine {
                x: shape.arrowWidth
                y: shape.centerThickness
            }
            PathLine {
                x: root.width - shape.arrowWidth
                y: shape.centerThickness
            }
            PathLine {
                x: root.width;
                y: shape.arrowHeight
            }
            PathLine {
                x: root.width;
                y: -(shape.arrowHeight + shape.listViewSpacing)
            }
            PathLine {
                x: root.width - shape.arrowWidth;
                y: -(shape.centerThickness + shape.listViewSpacing)
            }
            PathLine {
                x: shape.arrowWidth;
                y: -(shape.centerThickness + shape.listViewSpacing)
            }
            PathLine {
                x: 0;
                y: -(shape.arrowHeight + shape.listViewSpacing)
            }
        }
    }

    TapHandler {
        id: tapHandler
        acceptedButtons: Qt.LeftButton | Qt.RightButton | Qt.MiddleButton
        onSingleTapped: function(eventPoint, mouseButton) {
            switch (mouseButton) {
            case Qt.LeftButton:
                switch (point.modifiers) {
                case Qt.ControlModifier:
                    root.selectItem(root.index, PlaylistFilterProxyModel.Toggle)
                    break
                case Qt.ShiftModifier:
                    root.selectItem(root.index, PlaylistFilterProxyModel.Range)
                    break
                case Qt.AltModifier:
                    root.selectItem(root.index, PlaylistFilterProxyModel.ClearSingle)
                    break
                default:
                    if (PlaylistSettings.openWithSingleClick) {
                        setPlayingItem(root.index)
                        root.selectItem(root.index, PlaylistFilterProxyModel.Clear)
                    }
                    else {
                        root.selectItem(root.index, PlaylistFilterProxyModel.ClearSingle)
                    }
                    break
                }
                break
            case Qt.RightButton:
                // If right-clicked on a selected item, this will do no-op. Otherwise it will only select this item.
                root.selectItem(root.index, PlaylistFilterProxyModel.Single)
                openContextMenu()
                break
           case Qt.MiddleButton:
                scrollToPlayingItem()
                break
            }
        }

        onDoubleTapped: function(eventPoint, mouseButton) {
            if (mouseButton === Qt.LeftButton) {
                if (!PlaylistSettings.openWithSingleClick) {
                    setPlayingItem(root.index)
                    root.selectItem(root.index, PlaylistFilterProxyModel.Clear)
                }
            }
        }

        function scrollToPlayingItem() {
            if (root.m_mpv.visibleFilterProxyModel === root.m_mpv.activeFilterProxyModel) {
                const index = root.m_mpv.visibleFilterProxyModel.getPlayingItem()
                root.ListView.view.positionViewAtIndex(index, ListView.Beginning)
            }
        }

        function openContextMenu() {
            root.ListView.view.openContextMenu(root)
        }

        function setPlayingItem(pIndex) {
            root.m_mpv.visibleFilterProxyModel.setPlayingItem(pIndex)
        }
    }

    HoverHandler {
        id: hoverHandler
    }

    function pad(number: string, length: int) : string {
        while (number.length < length) {
            number = "0" + number
        }
        return number
    }

    function padRowNumberAsString() {
        return pad(root.rowNumber, root.ListView.view.count.toString().length)
    }

    function getLabelColor() {
        if (root.state === "Hovered") {
            return Kirigami.Theme.highlightedTextColor
        }
        if (root.state === "Highlighted") {
            return Kirigami.Theme.highlightedTextColor
        }
        return Kirigami.Theme.textColor
    }

    function getFontSize() {
        if (Window.window === null) {
            return 13
        }

        const mainWindow = Window.window as Main
        if (mainWindow.isFullScreen() && PlaylistSettings.bigFontFullscreen) {
            return 19
        } else {
            return 13
        }
    }

    function moveItems(pFrom, pTo) {
        root.m_mpv.visibleFilterProxyModel.moveItems(pFrom, pTo)
    }

    function selectItem(pIndex, pSelectionType) {
        root.m_mpv.visibleFilterProxyModel.selectItem(pIndex, pSelectionType)
    }

    function cacheItem() {
        // ListView::reuseItems=true, pools invisible items. During a drag, sometimes
        // the dragged item gets out of vision and pooled, breaking the drag behaviour.
        // This prevents pooling of the dragged item.
        ListView.view.currentIndex = index
    }
}
