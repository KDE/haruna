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
import Haruna.Components

ItemDelegate {
    id: root

    property bool isLocal: model.isLocal
    property string rowNumber: (index + 1).toString()
    property var alpha: PlaylistSettings.overlayVideo ? 0.6 : 1
    property int fontSize: (window.isFullScreen() && PlaylistSettings.bigFontFullscreen)
                           ? Kirigami.Units.gridUnit
                           : Kirigami.Units.gridUnit - 6

    padding: 0

    contentItem: Item {
        anchors.fill: parent
        RowLayout {
            anchors.fill: parent
            spacing: Kirigami.Units.largeSpacing
            Label {
                text: pad(root.rowNumber, playlistView.count.toString().length)
                visible: PlaylistSettings.showRowNumber
                font.pointSize: root.fontSize
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
                visible: model.isPlaying
                Layout.leftMargin: PlaylistSettings.showRowNumber ? 0 : Kirigami.Units.largeSpacing
            }

            LabelWithTooltip {
                color: Kirigami.Theme.textColor
                horizontalAlignment: Qt.AlignLeft
                verticalAlignment: Qt.AlignVCenter
                elide: Text.ElideRight
                font.pointSize: root.fontSize
                font.weight: model.isPlaying ? Font.ExtraBold : Font.Normal
                text: PlaylistSettings.showMediaTitle ? model.title : model.name
                layer.enabled: true
                Layout.fillWidth: true
                Layout.leftMargin: PlaylistSettings.showRowNumber || model.isPlaying ? 0 : Kirigami.Units.largeSpacing
                Binding {
                    target: root
                    property: "implicitHeight"
                    value: root.fontSize * 3
                }
            }

            Label {
                text: model.duration
                visible: model.duration.length > 0
                font.pointSize: root.fontSize
                horizontalAlignment: Qt.AlignCenter
                Layout.margins: Kirigami.Units.largeSpacing
            }
        }
    }
    onDoubleClicked: {
        mpv.playlistProxyModel.setPlayingItem(index)
        mpv.loadFile(path)
        mpv.pause = false
    }
}
