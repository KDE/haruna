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
    implicitHeight: (getFontSize() * 3)

    contentItem: Rectangle {
        width: root.width

        RowLayout {
            anchors.fill: parent
            spacing: Kirigami.Units.largeSpacing

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

            Label {
                text: root.padRowNumberAsString()
                color: root.getLabelColor()
                visible: PlaylistSettings.showRowNumber
                font.pointSize: root.getFontSize()
                horizontalAlignment: Qt.AlignCenter
                verticalAlignment: Qt.AlignVCenter
            }

            Rectangle {
                color: Qt.alpha(Kirigami.Theme.alternateBackgroundColor, 0.6)
                visible: PlaylistSettings.showRowNumber
                Layout.preferredWidth: 1
                Layout.fillHeight: true
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
                color: root.getLabelColor()
                horizontalAlignment: Qt.AlignLeft
                verticalAlignment: Qt.AlignVCenter
                elide: Text.ElideRight
                font.pointSize: root.getFontSize()
                text: PlaylistSettings.showMediaTitle ? root.title : root.name
                layer.enabled: true
                Layout.fillWidth: true
                Layout.leftMargin: PlaylistSettings.showRowNumber || root.isPlaying ? 0 : Kirigami.Units.largeSpacing
            }

            Label {
                text: root.duration.length > 0 ? root.duration : ""
                color: root.getLabelColor()
                font.pointSize: root.getFontSize()
                Layout.margins: Kirigami.Units.largeSpacing
            }
        }
    }
}
