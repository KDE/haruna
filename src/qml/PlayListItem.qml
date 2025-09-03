/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.13
import QtQuick.Controls 2.13
import QtQuick.Window 2.13
import org.kde.kirigami 2.11 as Kirigami

Item {
    id: root

    required property int row
    required property int column
    required property var model

    property int rowHeight: 30
    property string path: model.path

    implicitHeight: rowHeight + label.font.pointSize / 1.5

    Rectangle {
        anchors.fill: parent

        color: {
            if (root.model.isHovered && root.model.isPlaying) {
                let color = Kirigami.Theme.backgroundColor
                Qt.hsla(color.hslHue, color.hslSaturation, color.hslLightness, 0.8)
            } else if (root.model.isHovered && !root.model.isPlaying) {
                let color = Kirigami.Theme.backgroundColor
                Qt.hsla(color.hslHue, color.hslSaturation, color.hslLightness, 0.8)
            } else if (!root.model.isHovered && root.model.isPlaying) {
                Kirigami.Theme.highlightColor
            } else {
                let color = Kirigami.Theme.alternateBackgroundColor
                Qt.hsla(color.hslHue, color.hslSaturation, color.hslLightness, 0.7)
            }
        }

        Label {
            id: label

            anchors.fill: parent
            color: Kirigami.Theme.textColor
            horizontalAlignment: root.column === 0 ? Qt.AlignLeft : Qt.AlignCenter
            verticalAlignment: Qt.AlignVCenter
            elide: Text.ElideRight
            font.bold: true
            text: root.model.name
            leftPadding: 10
            rightPadding: root.column === 1 ? scrollBar.width : 10
            layer.enabled: true
            font.pointSize: {
                if (Window.window.visibility === Window.FullScreen && playList.bigFont) {
                    return 18
                }
                return 12
            }
            ToolTip {
                id: toolTip
                delay: 250
                visible: false
                text: root.model.name
                font.pointSize: label.font.pointSize + 2
            }
        }
    }

    MouseArea {
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        anchors.fill: parent
        hoverEnabled: true

        onEntered: {
            playListModel.setHoveredVideo(root.row)
            if (root.column === 0 && label.truncated) {
                toolTip.visible = true
            }
        }

        onExited: {
            playListModel.clearHoveredVideo(root.row)
            toolTip.visible = false
        }

        onDoubleClicked: {
            if (mouse.button === Qt.LeftButton) {
                window.openFile(root.model.path, true, false)
                playListModel.setPlayingVideo(root.row)
            }
        }
    }
}
