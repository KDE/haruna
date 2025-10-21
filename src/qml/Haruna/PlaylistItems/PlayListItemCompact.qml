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

            Kirigami.ListItemDragHandle {
                id: dragHandle
                listItem: root.dragRect
                listView: root.ListView?.view
                onMoveRequested: function (oldIndex, newIndex) {
                    root.selectItem(oldIndex, PlaylistFilterProxyModel.Single)
                    root.moveItems(oldIndex, newIndex)
                }
                Layout.alignment: Qt.AlignCenter
                Layout.leftMargin: Kirigami.Units.largeSpacing

                onDragActiveChanged: {
                    root.cacheItem()
                }
            }

            Kirigami.IconTitleSubtitle {
                icon.name: root.isPlaying ? "media-playback-start" : ""
                icon.color: root.getLabelColor()
                title: root.mainText()
                subtitle: root.duration
                color: root.getLabelColor()
                ToolTip.text: root.title
                ToolTip.visible: root.state === "Hovered"
                elide: Text.ElideRight
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
