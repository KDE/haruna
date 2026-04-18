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

    property real dragHandleWidth: Math.max(rowNumber.implicitWidth, dragHandle.implicitWidth)

    implicitHeight: (getFontSize() * 3)

    contentItem: Rectangle {
        width: root.width

        RowLayout {
            anchors.fill: parent
            spacing: Kirigami.Units.largeSpacing

            Item {
                clip: true
                Layout.alignment: Qt.AlignCenter
                Layout.fillHeight: true
                Layout.leftMargin: Kirigami.Units.largeSpacing
                Layout.bottomMargin: Kirigami.Units.largeSpacing
                Layout.topMargin: Kirigami.Units.largeSpacing

                Layout.preferredWidth: {
                    if (rowNumber.visible || root.hovered) {
                        return root.dragHandleWidth
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
                    spacing: 0

                    x: root.hovered ? 0 : -parent.width

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
                        width: root.dragHandleWidth

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
                        horizontalAlignment: Qt.AlignHCenter
                        anchors.verticalCenter: parent.verticalCenter
                        width: root.dragHandleWidth
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

            Kirigami.Icon {
                source: "media-playback-start"
                color: root.getLabelColor()
                visible: root.isPlaying
                Layout.preferredWidth: Kirigami.Units.iconSizes.small
                Layout.preferredHeight: Kirigami.Units.iconSizes.small
            }

            LabelWithTooltip {
                color: root.getLabelColor()
                horizontalAlignment: Qt.AlignLeft
                verticalAlignment: Qt.AlignVCenter
                elide: Text.ElideRight
                font.pointSize: root.getFontSize()
                text: PlaylistSettings.showMediaTitle ? root.title : root.name
                Layout.fillWidth: true
            }

            Label {
                text: root.duration.length > 0 ? root.duration : ""
                color: root.getLabelColor()
                font.pointSize: root.getFontSize()
                Layout.margins: Kirigami.Units.largeSpacing

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
                               ? root.m_mpv.position/root.m_mpv.duration * parent.width
                               : root.playbackPosition * parent.width
                        color: Kirigami.Theme.visitedLinkColor
                    }
                }

            }
        }
    }
}
