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

    required property int index
    required property string title
    required property string name
    required property string duration
    required property bool isLocal
    required property bool isPlaying

    property string rowNumber: (root.index + 1).toString()
    property var alpha: PlaylistSettings.overlayVideo ? 0.6 : 1
    property int fontSize: (window.isFullScreen() && PlaylistSettings.bigFontFullscreen)
                           ? Kirigami.Units.gridUnit
                           : Kirigami.Units.gridUnit - 6

    padding: 0
    implicitWidth: ListView.view.width
    highlighted: root.isPlaying

    background: Rectangle {
        anchors.fill: parent
        color: {
            if (hovered) {
                return Qt.alpha(Kirigami.Theme.hoverColor, alpha)
            }

            if (highlighted) {
                return Qt.alpha(Kirigami.Theme.highlightColor, alpha)
            }

            return Qt.alpha(Kirigami.Theme.backgroundColor, alpha)
        }
    }

    contentItem: RowLayout {
        width: parent.width
        spacing: Kirigami.Units.largeSpacing

        Label {
            text: pad(root.rowNumber, playlistView.count.toString().length)
            color: root.hovered || root.highlighted ? Kirigami.Theme.highlightedTextColor : Kirigami.Theme.textColor
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
            color: Kirigami.Theme.alternateBackgroundColor
            visible: PlaylistSettings.showRowNumber
            Layout.preferredWidth: 1
            Layout.fillHeight: true
        }

        Kirigami.Icon {
            source: "media-playback-start"
            color: root.hovered || root.highlighted ? Kirigami.Theme.highlightedTextColor : Kirigami.Theme.textColor
            height: Kirigami.Units.iconSizes.small
            visible: root.isPlaying
            Layout.preferredWidth: Kirigami.Units.iconSizes.small
            Layout.leftMargin: PlaylistSettings.showRowNumber ? 0 : Kirigami.Units.largeSpacing
        }

        LabelWithTooltip {
            color: root.hovered || root.highlighted ? Kirigami.Theme.highlightedTextColor : Kirigami.Theme.textColor
            horizontalAlignment: Qt.AlignLeft
            verticalAlignment: Qt.AlignVCenter
            elide: Text.ElideRight
            font.pointSize: root.fontSize
            text: PlaylistSettings.showMediaTitle ? root.title : root.name
            layer.enabled: true
            Layout.fillWidth: true
            Layout.leftMargin: PlaylistSettings.showRowNumber || root.isPlaying ? 0 : Kirigami.Units.largeSpacing
            Binding {
                target: root
                property: "implicitHeight"
                value: root.fontSize * 3
            }

        }

        Label {
            text: root.duration.length > 0 ? root.duration : ""
            color: root.hovered || root.highlighted ? Kirigami.Theme.highlightedTextColor : Kirigami.Theme.textColor
            font.pointSize: root.fontSize
            Layout.margins: Kirigami.Units.largeSpacing
        }
    }
    onClicked: {
        if (PlaylistSettings.openWithSingleClick) {
            mpv.playlistProxyModel.setPlayingItem(index)
        }
    }

    onDoubleClicked: {
        if (!PlaylistSettings.openWithSingleClick) {
            mpv.playlistProxyModel.setPlayingItem(index)
        }
    }
}
