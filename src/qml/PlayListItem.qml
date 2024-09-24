/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import org.kde.kirigami 2.11 as Kirigami

Rectangle {
    id: root

    property string path: model.path

    color: {
        if (model.isHovered && model.isPlaying) {
            let color = Kirigami.Theme.backgroundColor
            Qt.hsla(color.hslHue, color.hslSaturation, color.hslLightness, 0.8)
        } else if (model.isHovered && !model.isPlaying) {
            let color = Kirigami.Theme.backgroundColor
            Qt.hsla(color.hslHue, color.hslSaturation, color.hslLightness, 0.8)
        } else if (!model.isHovered && model.isPlaying) {
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
        horizontalAlignment: column === 0 ? Qt.AlignLeft : Qt.AlignCenter
        verticalAlignment: Qt.AlignVCenter
        elide: Text.ElideRight
        font.bold: true
        text: model.name
        leftPadding: 10
        rightPadding: 10
        layer.enabled: true
        font.pointSize: tableView.rowFontSize
        ToolTip {
            id: toolTip
            delay: 250
            visible: false
            text: model.name
            font.pointSize: label.font.pointSize + 2
        }
    }

    MouseArea {
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        anchors.fill: parent
        hoverEnabled: true

        onEntered: {
            playListModel.setHoveredVideo(row)
            if (column === 0 && label.truncated) {
                toolTip.visible = true
            }
        }

        onExited: {
            playListModel.clearHoveredVideo(row)
            toolTip.visible = false
        }

        onDoubleClicked: function(mouse) {
            if (mouse.button === Qt.LeftButton) {
                window.openFile(model.path, true, false)
                playListModel.setPlayingVideo(row)
            }
        }
    }
}
