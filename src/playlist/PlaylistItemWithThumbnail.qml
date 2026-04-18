/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.haruna
import org.kde.haruna.settings

PlaylistItemDelegate {
    id: root
    implicitHeight: (Kirigami.Units.gridUnit - 6) * 8

    contentItem: Rectangle {
        width: root.width

        RowLayout {
            anchors.fill: parent
            spacing: Kirigami.Units.largeSpacing
            Layout.alignment: Qt.AlignVCenter
            anchors.rightMargin: Kirigami.Units.largeSpacing

            Item {
                clip: true
                Layout.alignment: Qt.AlignCenter
                Layout.fillHeight: true
                Layout.leftMargin: Kirigami.Units.largeSpacing
                Layout.bottomMargin: Kirigami.Units.largeSpacing
                Layout.topMargin: Kirigami.Units.largeSpacing

                Layout.preferredWidth: {
                    if (rowNumber.visible) {
                        return Math.max(dragHandle.implicitWidth, rowNumber.implicitWidth)
                    }
                    if (root.hovered) {
                        return dragHandle.implicitWidth
                    }
                    return 0
                }

                Behavior on Layout.preferredWidth {
                    NumberAnimation {
                        duration: Kirigami.Units.longDuration
                        easing.type: Easing.OutCubic
                    }
                }

                Row {
                    height: parent.height
                    spacing: Kirigami.Units.largeSpacing

                    x: {
                        if (rowNumber.visible) {
                            // Center to the row
                            if (root.hovered) {
                                return (parent.width - dragHandle.implicitWidth) * 0.5
                            }
                            return (parent.width - rowNumber.implicitWidth) * 0.5 - (dragHandle.implicitWidth + Kirigami.Units.largeSpacing)
                        }
                        else {
                            return 0
                        }
                    }

                    Behavior on x {
                        NumberAnimation {
                            duration: Kirigami.Units.longDuration
                            easing.type: Easing.OutCubic
                        }
                    }

                    Kirigami.ListItemDragHandle {
                        id: dragHandle

                        listItem: root.dragRect
                        listView: root.ListView?.view

                        onMoveRequested: function (oldIndex, newIndex) {
                            root.selectItem(oldIndex, PlaylistFilterProxyModel.Single)
                            root.moveItems(oldIndex, newIndex)
                        }

                        onDragActiveChanged: {
                            root.cacheItem()
                        }

                        anchors.verticalCenter: parent.verticalCenter
                    }

                    Label {
                        id: rowNumber

                        text: root.padRowNumberAsString()
                        color: root.getLabelColor()
                        visible: PlaylistSettings.showRowNumber
                        font.pointSize: root.getFontSize()
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }
            }

            Rectangle {
                color: Qt.alpha(Kirigami.Theme.textColor, 0.2)
                visible: PlaylistSettings.showRowNumber
                Layout.preferredWidth: 1
                Layout.fillHeight: true
                Layout.topMargin: Kirigami.Units.smallSpacing
                Layout.bottomMargin: Kirigami.Units.smallSpacing
            }

            Item {
                Layout.preferredWidth: root.height * 1.33
                Layout.preferredHeight: root.height

                Image {
                    anchors.fill: parent
                    anchors.margins: Kirigami.Units.mediumSpacing
                    source: "image://thumbnail/%1".arg(root.path)
                    sourceSize.width: parent.width
                    sourceSize.height: parent.height
                    asynchronous: true
                    fillMode: Image.PreserveAspectCrop

                    Rectangle {
                        visible: root.duration.length > 0
                        height: root.getImageFontSize() + (Kirigami.Units.smallSpacing * 2) + 4
                        anchors.left: parent.left
                        anchors.bottom: parent.bottom
                        anchors.right: parent.right
                        color: Qt.alpha(Kirigami.Theme.backgroundColor, 0.7)

                        ColumnLayout {
                            anchors.fill: parent

                            Label {
                                Layout.alignment: Qt.AlignCenter
                                Layout.fillHeight: true
                                color: Kirigami.Theme.textColor
                                horizontalAlignment: Qt.AlignCenter
                                text: root.duration
                                font.pointSize: root.getImageFontSize()
                            }

                            Rectangle {
                                visible: PlaybackSettings.restoreFilePosition
                                color: Qt.alpha(Kirigami.Theme.visitedLinkColor, 1.0)
                                Layout.preferredWidth: root.isPlaying
                                                       ? root.m_mpv.position/root.m_mpv.duration * parent.width
                                                       : root.playbackPosition * parent.width
                                Layout.preferredHeight: 2
                                Layout.alignment: Qt.AlignLeft
                            }
                        }
                    }
                }
            }

            Kirigami.Icon {
                source: "media-playback-start"
                color: root.getLabelColor()
                visible: root.isPlaying
                Layout.preferredWidth: Kirigami.Units.iconSizes.small
                Layout.preferredHeight: Kirigami.Units.iconSizes.small
                Layout.leftMargin: PlaylistSettings.showRowNumber ? 0 : Kirigami.Units.largeSpacing
            }

            LabelWithTooltip {
                text: PlaylistSettings.showMediaTitle ? root.title : root.name
                color: root.getLabelColor()
                horizontalAlignment: Qt.AlignLeft
                verticalAlignment: Qt.AlignVCenter
                elide: Text.ElideRight
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                font.pointSize: root.getLabelFontSize()
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.topMargin: Kirigami.Units.largeSpacing
                Layout.bottomMargin: Kirigami.Units.largeSpacing
                Layout.leftMargin: PlaylistSettings.showRowNumber || root.isPlaying ? 0 : Kirigami.Units.largeSpacing
            }
        }
    }

    function getImageFontSize() {
        if (Window.window === null) {
            return Kirigami.Units.gridUnit - 2
        }

        const mainWindow = Window.window as Main
        if (mainWindow.isFullScreen() && PlaylistSettings.bigFontFullscreen) {
            return Kirigami.Units.gridUnit - 2
        } else {
            return Kirigami.Units.gridUnit - 5
        }
    }

    function getLabelFontSize() {
        if (Window.window === null) {
            return Kirigami.Units.gridUnit
        }

        const mainWindow = Window.window as Main
        if (mainWindow.isFullScreen() && PlaylistSettings.bigFontFullscreen) {
            return Kirigami.Units.gridUnit
        } else {
            return Kirigami.Units.gridUnit - 5
        }
    }
}
