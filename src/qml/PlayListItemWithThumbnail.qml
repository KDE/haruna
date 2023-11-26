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

    implicitWidth: ListView.view.width
    implicitHeight: (Kirigami.Units.gridUnit - 6) * 8
    padding: 0

    onDoubleClicked: {
        mpv.playlistProxyModel.setPlayingItem(index)
        mpv.loadFile(path)
        mpv.pause = false
    }

    contentItem: Item {
        anchors.fill: parent
        RowLayout {
            anchors.fill: parent
            anchors.rightMargin: Kirigami.Units.largeSpacing
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

            Item {
                width: (root.height - 20) * 1.33
                height: root.height - 20

                Image {
                    anchors.fill: parent
                    source: "image://thumbnail/" + model.path
                    sourceSize.width: parent.width
                    sourceSize.height: parent.height
                    width: parent.width
                    height: parent.height
                    asynchronous: true
                    fillMode: Image.PreserveAspectCrop

                    Rectangle {
                        visible: model.duration.length > 0
                        height: 25
                        anchors.left: parent.left
                        anchors.bottom: parent.bottom
                        anchors.right: parent.right
                        color: Kirigami.Theme.alternateBackgroundColor

                        Label {
                            anchors.centerIn: parent
                            color: Kirigami.Theme.textColor
                            horizontalAlignment: Qt.AlignCenter
                            text: model.duration
                            font.pointSize: (window.isFullScreen() && playList.bigFont)
                                            ? Kirigami.Units.gridUnit
                                            : Kirigami.Units.gridUnit - 5

                            Layout.margins: Kirigami.Units.largeSpacing
                        }
                    }
                }
            }

            Kirigami.Icon {
                source: "media-playback-start"
                width: Kirigami.Units.iconSizes.small
                height: Kirigami.Units.iconSizes.small
                visible: model.isPlaying

                Layout.leftMargin: PlaylistSettings.showRowNumber ? 0 : Kirigami.Units.largeSpacing
            }

            LabelWithTooltip {
                text: PlaylistSettings.showMediaTitle ? model.title : model.name
                color: Kirigami.Theme.textColor
                horizontalAlignment: Qt.AlignLeft
                verticalAlignment: Qt.AlignVCenter
                elide: Text.ElideRight
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                font.pointSize: (window.isFullScreen() && playList.bigFont)
                                ? Kirigami.Units.gridUnit
                                : Kirigami.Units.gridUnit - 5
                font.weight: model.isPlaying ? Font.ExtraBold : Font.Normal
                layer.enabled: true
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.topMargin: Kirigami.Units.largeSpacing
                Layout.bottomMargin: Kirigami.Units.largeSpacing
                Layout.leftMargin: PlaylistSettings.showRowNumber || model.isPlaying ? 0 : Kirigami.Units.largeSpacing
            }
        }
    }
}
