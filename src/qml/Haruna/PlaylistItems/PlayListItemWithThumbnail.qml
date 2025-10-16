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
                Layout.fillHeight: true

                onDragActiveChanged: {
                    root.cacheItem()
                }
            }

            Label {
                text: root.padRowNumberAsString()
                color: root.getLabelColor()
                visible: PlaylistSettings.showRowNumber
                font.pointSize: root.getFontSize()
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            Rectangle {
                color: Qt.alpha(Kirigami.Theme.alternateBackgroundColor, 0.6)
                visible: PlaylistSettings.showRowNumber
                Layout.preferredWidth: 1
                Layout.fillHeight: true
            }

            Item {
                Layout.preferredWidth: root.height * 1.33
                Layout.preferredHeight: root.height

                Image {
                    anchors.fill: parent
                    anchors.margins: Kirigami.Units.largeSpacing
                    source: "image://thumbnail/%1".arg(root.path)
                    sourceSize.width: parent.width
                    sourceSize.height: parent.height
                    width: parent.width
                    height: parent.height
                    asynchronous: true
                    fillMode: Image.PreserveAspectCrop

                    Rectangle {
                        visible: root.duration.length > 0
                        height: durationLabel.font.pointSize + (Kirigami.Units.smallSpacing * 2)
                        anchors.left: parent.left
                        anchors.bottom: parent.bottom
                        anchors.right: parent.right
                        color: Qt.alpha(Kirigami.Theme.backgroundColor, 0.7)

                        Label {
                            id: durationLabel

                            anchors.centerIn: parent
                            anchors.margins: Kirigami.Units.largeSpacing
                            color: Kirigami.Theme.textColor
                            horizontalAlignment: Qt.AlignCenter
                            text: root.duration
                            font.pointSize: root.getImageFontSize()
                            Layout.margins: Kirigami.Units.largeSpacing
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
