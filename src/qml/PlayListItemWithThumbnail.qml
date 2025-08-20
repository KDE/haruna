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

ItemDelegate {
    id: root

    required property MpvVideo m_mpv

    required property int index
    required property string title
    required property string name
    required property string path
    required property string duration
    required property bool isLocal
    required property bool isPlaying

    property string rowNumber: (root.index + 1).toString()
    property var alpha: PlaylistSettings.overlayVideo ? 0.6 : 1

    implicitWidth: ListView.view.width
    implicitHeight: (Kirigami.Units.gridUnit - 6) * 8
    padding: 0
    highlighted: root.isPlaying

    onClicked: {
        if (PlaylistSettings.openWithSingleClick) {
            root.m_mpv.playlistFilterProxyModel.setPlayingItem(index)
        }
    }

    onDoubleClicked: {
        if (!PlaylistSettings.openWithSingleClick) {
            root.m_mpv.playlistFilterProxyModel.setPlayingItem(index)
        }
    }

    background: Rectangle {
        anchors.fill: parent
        color: {
            if (root.hovered) {
                return Qt.alpha(Kirigami.Theme.hoverColor, root.alpha)
            }

            if (root.highlighted) {
                return Qt.alpha(Kirigami.Theme.highlightColor, root.alpha)
            }

            return Qt.alpha(Kirigami.Theme.backgroundColor, root.alpha)
        }
    }

    contentItem: Item {
        anchors.fill: parent
        RowLayout {
            anchors.fill: parent
            anchors.rightMargin: Kirigami.Units.largeSpacing
            spacing: Kirigami.Units.largeSpacing

            Label {
                text: pad(root.rowNumber, root.ListView.view.count.toString().length)
                color: root.hovered || root.highlighted ? Kirigami.Theme.highlightedTextColor : Kirigami.Theme.textColor
                visible: PlaylistSettings.showRowNumber
                font.pointSize: {
                    if (Window.window === null) {
                        return Kirigami.Units.gridUnit
                    }

                    const mainWindow = Window.window as Main
                    if (mainWindow.isFullScreen() && PlaylistSettings.bigFontFullscreen) {
                        return Kirigami.Units.gridUnit
                    } else {
                        return Kirigami.Units.gridUnit - 6
                    }
                }
                horizontalAlignment: Qt.AlignCenter
                Layout.leftMargin: Kirigami.Units.largeSpacing

                function pad(number: string, length: int) : string {
                    while (number.length < length) {
                        number = "0" + number
                    }
                    return number
                }
            }

            Rectangle {
                color: Qt.alpha(Kirigami.Theme.alternateBackgroundColor, 0.6)
                visible: PlaylistSettings.showRowNumber
                Layout.preferredWidth: 1
                Layout.fillHeight: true
            }

            Item {
                Layout.preferredWidth: (root.height - 20) * 1.33
                Layout.preferredHeight: root.height - 20

                Image {
                    anchors.fill: parent
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
                            color: Kirigami.Theme.highlightedTextColor
                            horizontalAlignment: Qt.AlignCenter
                            text: root.duration
                            font.pointSize: {
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

                            Layout.margins: Kirigami.Units.largeSpacing
                        }
                    }
                }
            }

            Kirigami.Icon {
                source: "media-playback-start"
                color: root.hovered || root.highlighted ? Kirigami.Theme.highlightedTextColor : Kirigami.Theme.textColor
                visible: root.isPlaying
                Layout.preferredWidth: Kirigami.Units.iconSizes.small
                Layout.preferredHeight: Kirigami.Units.iconSizes.small

                Layout.leftMargin: PlaylistSettings.showRowNumber ? 0 : Kirigami.Units.largeSpacing
            }

            LabelWithTooltip {
                text: PlaylistSettings.showMediaTitle ? root.title : root.name
                color: root.hovered || root.highlighted ? Kirigami.Theme.highlightedTextColor : Kirigami.Theme.textColor
                horizontalAlignment: Qt.AlignLeft
                verticalAlignment: Qt.AlignVCenter
                elide: Text.ElideRight
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                font.pointSize: {
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
                layer.enabled: true
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.topMargin: Kirigami.Units.largeSpacing
                Layout.bottomMargin: Kirigami.Units.largeSpacing
                Layout.leftMargin: PlaylistSettings.showRowNumber || root.isPlaying ? 0 : Kirigami.Units.largeSpacing
            }
        }
    }
}
