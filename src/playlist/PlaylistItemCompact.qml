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
    implicitHeight: Kirigami.Units.iconSizes.medium

    contentItem: Rectangle {
        width: root.width

        RowLayout {
            anchors.fill: parent
            anchors.rightMargin: Kirigami.Units.mediumSpacing
            Layout.maximumWidth: root.width - Kirigami.Units.largeSpacing * 4

            Item {
                clip: true
                Layout.alignment: Qt.AlignCenter
                Layout.fillHeight: true
                Layout.leftMargin: Kirigami.Units.largeSpacing
                Layout.bottomMargin: Kirigami.Units.largeSpacing
                Layout.topMargin: Kirigami.Units.largeSpacing

                Layout.preferredWidth: {
                    if (icon.visible) {
                        return Math.max(dragHandle.implicitWidth, icon.width)
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
                        if (icon.visible) {
                            // Center to the row
                            if (root.hovered) {
                                return (parent.width - dragHandle.implicitWidth) * 0.5
                            }
                            return (parent.width - icon.width) * 0.5 - (dragHandle.implicitWidth + Kirigami.Units.largeSpacing)
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

                    Kirigami.Icon {
                        id: icon

                        source: "media-playback-start"
                        color: root.getLabelColor()
                        visible: root.isPlaying
                        width: Kirigami.Units.iconSizes.small
                        height: Kirigami.Units.iconSizes.small

                        anchors.verticalCenter: parent.verticalCenter
                    }
                }
            }

            Kirigami.TitleSubtitle {
                color: root.getLabelColor()
                elide: Text.ElideRight
                subtitle: root.duration
                title: root.mainText()
                ToolTip.text: root.title
                ToolTip.visible: root.hovered
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
            }
        }
    }

    function mainText() : string {
        const rowNumber = "%1. ".arg(root.padRowNumberAsString())

        if (PlaylistSettings.showRowNumber) {
            return rowNumber + (PlaylistSettings.showMediaTitle ? root.title : root.name)
        }
        return (PlaylistSettings.showMediaTitle ? root.title : root.name)
    }
}
