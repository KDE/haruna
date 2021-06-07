/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.12

import org.kde.kirigami 2.11 as Kirigami
import org.kde.haruna 1.0
import Haruna.Components 1.0

Kirigami.BasicListItem {
    id: root

    property bool isPlaying: model.isPlaying
    property string rowNumber: (index + 1).toString()
    property var alpha: PlaylistSettings.overlayVideo ? 0.6 : 1

    height: label.font.pointSize * 3 + PlaylistSettings.rowHeight
    padding: 0
    backgroundColor: {
        let color = model.isPlaying ? Kirigami.Theme.highlightColor : Kirigami.Theme.backgroundColor
        Qt.hsla(color.hslHue, color.hslSaturation, color.hslLightness, alpha)
    }

    contentItem: Rectangle {
        anchors.fill: parent
        color: "transparent"
        RowLayout {
            anchors.fill: parent
            spacing: Kirigami.Units.largeSpacing
            Label {
                text: pad(root.rowNumber, playlistView.count.toString().length)
                visible: PlaylistSettings.showRowNumber
                font.pointSize: (window.isFullScreen() && playList.bigFont)
                                ? Kirigami.Units.gridUnit
                                : Kirigami.Units.gridUnit - 6
                horizontalAlignment: Qt.AlignCenter
                Layout.leftMargin: Kirigami.Units.largeSpacing

                function pad(number, length) {
                    while (number.length < length)
                        number = "0" + number;
                    return number;
                }

            }

            Rectangle {
                width: 1
                color: Kirigami.Theme.alternateBackgroundColor
                visible: PlaylistSettings.showRowNumber
                Layout.fillHeight: true
            }

            Kirigami.Icon {
                source: "media-playback-start"
                width: Kirigami.Units.iconSizes.small
                height: Kirigami.Units.iconSizes.small
                visible: isPlaying
                Layout.leftMargin: PlaylistSettings.showRowNumber ? 0 : Kirigami.Units.largeSpacing
            }

            LabelWithTooltip {
                id: label

                toolTipFontSize: label.font.pointSize + 2

                color: Kirigami.Theme.textColor
                horizontalAlignment: Qt.AlignLeft
                verticalAlignment: Qt.AlignVCenter
                elide: Text.ElideRight
                font.pointSize: (window.isFullScreen() && playList.bigFont)
                                ? Kirigami.Units.gridUnit
                                : Kirigami.Units.gridUnit - 6
                font.weight: isPlaying ? Font.ExtraBold : Font.Normal
                text: PlaylistSettings.showMediaTitle ? model.title : model.name
                layer.enabled: true
                Layout.fillWidth: true
                Layout.leftMargin: PlaylistSettings.showRowNumber || isPlaying ? 0 : Kirigami.Units.largeSpacing
            }

            Label {
                text: model.duration
                visible: model.duration.length > 0
                font.pointSize: (window.isFullScreen() && playList.bigFont)
                                ? Kirigami.Units.gridUnit
                                : Kirigami.Units.gridUnit - 6
                horizontalAlignment: Qt.AlignCenter
                Layout.margins: Kirigami.Units.largeSpacing
            }
        }
    }
    onDoubleClicked: {
        mpv.playlistModel.setPlayingVideo(index)
        mpv.loadFile(path, !isYouTubePlaylist)
        mpv.pause = false
    }
}
