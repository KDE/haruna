/*
 * SPDX-FileCopyrightText: 2026 George Florea Bănuș <georgefb899@gmail.com>
 * SPDX-FileCopyrightText: 2025 Muhammet Sadık Uğursoy <sadikugursoy@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Shapes

import org.kde.kirigami as Kirigami

import org.kde.haruna
import org.kde.haruna.playlist
import org.kde.haruna.settings

Loader {
    id: root

    required property int index
    required property string rowNumber
    required property string titleWithFallback
    required property string nameWithFallbackRole
    required property double duration
    required property string formattedDuration
    required property string path
    required property bool isLocal
    required property bool isPlaying
    required property bool isSelected
    required property string section
    required property real playbackPosition

    required property MpvItem mpvItem
    required property PlaylistsManager playlistsManager

    property bool highlighted: root.isPlaying || root.isSelected
    property bool removeAnimationRunning: false
    property int fontSizeAdjustment: PlaylistSettings.bigFontFullscreen ? 5 : 0
    property int labelFontSize: (Window.window as Main)?.isFullScreen()
                                ? Kirigami.Theme.defaultFont.pointSize + fontSizeAdjustment
                                : Kirigami.Theme.defaultFont.pointSize
    property color labelFontColor: highlighted ? Kirigami.Theme.highlightedTextColor : Kirigami.Theme.textColor

    function moveItems(from: int, to: int) : void {
        root.playlistsManager.visiblePlaylist.moveItems(from, to)
    }

    function selectItem(index: int, selectionType: int) : void {
        root.playlistsManager.visiblePlaylist.selectItem(index, selectionType)
    }

    function setPlayingItem(index: int) : void {
        root.playlistsManager.visiblePlaylist.setPlayingItem(index)
    }

    function scrollToPlayingItem() : void {
        if (root.playlistsManager.visiblePlaylist === root.playlistsManager.activePlaylist) {
            const index = root.playlistsManager.visiblePlaylist.getPlayingItem()
            root.ListView.view.positionViewAtIndex(index, ListView.Beginning)
        }
    }

    ListView.onRemove: {
        removeAnimation.start()
    }

    Item {
        z: 100
        anchors.left: root.left
        anchors.top: root.top
        anchors.bottom: root.bottom
        width: 50

        HoverHandler {
            id: showDragHandleHH
        }
    }

    SequentialAnimation {
        id: removeAnimation

        PropertyAction { target: root; property: "ListView.delayRemove"; value: true }
        PropertyAction { target: root; property: "removeAnimationRunning"; value: true }

        ParallelAnimation {
            NumberAnimation { target: root; property: "height"; to: 0; duration: Kirigami.Units.shortDuration }
            NumberAnimation { target: root; property: "opacity"; to: 0; duration: Kirigami.Units.shortDuration }
        }

        PropertyAction { target: root; property: "ListView.delayRemove"; value: false }
        PropertyAction { target: root; property: "removeAnimationRunning"; value: false }

        onFinished: {
            root.opacity = 1.0
            root.height = Qt.binding(function() { return root.implicitHeight })
        }
    }

    sourceComponent: ItemDelegate {
        id: itemDelegate

        implicitWidth: root.ListView.view.width
        height: PlaylistSettings.style == "withThumbnails"
                ? (Kirigami.Units.gridUnit - 6) * 8
                : implicitHeight
        highlighted: root.highlighted
        topPadding: Kirigami.Units.smallSpacing
        bottomPadding: Kirigami.Units.smallSpacing

        contentItem: RowLayout {
            id: content

            width: itemDelegate.width

            NumberAndDragHandleItem {
                delegate: itemDelegate
            }

            Loader {
                active: PlaylistSettings.style == "withThumbnails"
                visible: active
                sourceComponent: ThumbnailItem {}
                Layout.alignment: Qt.AlignCenter
            }

            Kirigami.Icon {
                source: "media-playback-start"
                color: root.labelFontColor
                visible: root.isPlaying
                Layout.preferredWidth: Kirigami.Units.iconSizes.small
                Layout.preferredHeight: Kirigami.Units.iconSizes.small
            }

            LabelWithTooltip {
                text: PlaylistSettings.showMediaTitle ? root.titleWithFallback : root.nameWithFallbackRole
                elide: Text.ElideRight
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                font.pointSize: root.labelFontSize
                color: root.labelFontColor
                horizontalAlignment: Qt.AlignLeft
                verticalAlignment: Qt.AlignVCenter
                maximumLineCount: PlaylistSettings.style == "withThumbnails" ? 3 : 1

                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            Label {
                text: root.formattedDuration.length > 0 ? root.formattedDuration : ""
                font.pointSize: root.labelFontSize
                color: root.labelFontColor
                visible: PlaylistSettings.style !== "withThumbnails"

                Layout.margins: Kirigami.Units.smallSpacing

                Rectangle {
                    visible: PlaybackSettings.restoreFilePosition
                    anchors.right: parent.right
                    anchors.top: parent.bottom
                    height: 2
                    width: parent.width
                    color: Kirigami.Theme.visitedLinkBackgroundColor

                    Rectangle {
                        height: 2
                        width: root.isPlaying
                               ? root.mpvItem.position/root.duration * parent.width
                               : root.playbackPosition * parent.width
                        color: Kirigami.Theme.visitedLinkColor
                    }
                }
            }
        }

        ContextMenu.menu: PlaylistItemContextMenu {
            index: root.index
            isLocal: root.isLocal
            playlistsManager: root.playlistsManager
        }
        ContextMenu.onRequested: root.selectItem(root.index, PlaylistFilterProxyModel.Single)

        TapHandler {
            id: tapHandler

            acceptedButtons: Qt.LeftButton | Qt.MiddleButton
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
                            root.setPlayingItem(root.index)
                        }
                        root.selectItem(root.index, PlaylistFilterProxyModel.ClearSingle)
                        break
                    }
                    break
                case Qt.MiddleButton:
                    root.scrollToPlayingItem()
                    break
                }
            }

            onDoubleTapped: function(eventPoint, mouseButton) {
                if (mouseButton === Qt.LeftButton) {
                    if (!PlaylistSettings.openWithSingleClick) {
                        root.setPlayingItem(root.index)
                        root.selectItem(root.index, PlaylistFilterProxyModel.ClearSingle)
                        Models.recentFilesModel.addRecentFile(root.playlistsManager.activeItemPath(), RecentFilesModel.OpenedFrom.Playlist)
                    }
                }
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
                root.playlistsManager.visiblePlaylist.addFilesAndFolders(drop.urls, PlaylistModel.Insert, root.index)
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
    }

    component ThumbnailItem: Item {
        visible: !root.removeAnimationRunning
        implicitWidth: visible ? root.height * 1.33
                                 // keeps text at the same x position during the remove animation
                               : (Kirigami.Units.gridUnit - 6) * 8 * 1.33
        implicitHeight: root.height - Kirigami.Units.smallSpacing * 2

        Image {
            anchors.fill: parent
            source: "image://thumbnail/%1".arg(root.path)
            sourceSize.width: parent.width
            sourceSize.height: parent.height
            asynchronous: true
            fillMode: Image.PreserveAspectCrop

            Rectangle {
                visible: root.formattedDuration.length > 0
                height: thumbDurationLayout.height
                anchors.left: parent.left
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                color: Qt.alpha(Kirigami.Theme.backgroundColor, 0.7)

                ColumnLayout {
                    id: thumbDurationLayout

                    width: parent.width
                    spacing: 1

                    Label {
                        id: thumbDurationLabel

                        Layout.alignment: Qt.AlignCenter
                        Layout.fillHeight: true
                        color: Kirigami.Theme.textColor
                        horizontalAlignment: Qt.AlignCenter
                        text: root.formattedDuration
                    }

                    Rectangle {
                        visible: PlaybackSettings.restoreFilePosition
                        color: Qt.alpha(Kirigami.Theme.visitedLinkColor, 1.0)
                        Layout.preferredWidth: root.isPlaying
                                               ? root.mpvItem.position/root.mpvItem.duration * parent.width
                                               : root.playbackPosition * parent.width
                        Layout.preferredHeight: 2
                        Layout.alignment: Qt.AlignLeft
                    }
                }
            }
        }
    }

    component NumberAndDragHandleItem: Item {
        id: numberAndDragHandleComponent

        required property ItemDelegate delegate

        clip: true

        Layout.fillHeight: true
        Layout.preferredWidth: rowNumberItem.visible || showDragHandleHH.hovered
                               ? Math.max(rowNumberItem.implicitWidth, dragHandle.implicitWidth)
                               : 0
        Layout.rightMargin: Kirigami.Units.largeSpacing

        Behavior on Layout.preferredWidth {
            NumberAnimation {
                duration: Kirigami.Units.longDuration
                easing.type: Easing.OutCubic
            }
        }

        Row {
            width: numberAndDragHandleComponent.Layout.preferredWidth
            height: parent.height
            spacing: 0
            x: showDragHandleHH.hovered ? 0 : -width

            Behavior on x {
                NumberAnimation {
                    duration: Kirigami.Units.longDuration
                    easing.type: Easing.OutCubic
                }
            }

            Kirigami.ListItemDragHandle {
                id: dragHandle

                width: numberAndDragHandleComponent.Layout.preferredWidth
                height: parent.height

                listItem: numberAndDragHandleComponent.delegate
                listView: root.ListView?.view

                onMoveRequested: function (oldIndex, newIndex) {
                    root.selectItem(oldIndex, PlaylistFilterProxyModel.Single)
                    root.moveItems(oldIndex, newIndex)
                }

                anchors.verticalCenter: parent.verticalCenter
            }

            Label {
                id: rowNumberItem

                visible: PlaylistSettings.showRowNumber
                width: numberAndDragHandleComponent.Layout.preferredWidth
                height: parent.height

                text: root.rowNumber
                font.pointSize: root.labelFontSize
                color: root.labelFontColor
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
            }
        }
    }
}
