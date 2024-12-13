/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls

import org.kde.kirigami as Kirigami
import org.kde.haruna
import org.kde.haruna.settings

ItemDelegate {
    id: root

    required property MpvVideo m_mpv

    required property int index
    required property string title
    required property string name
    required property string duration
    required property bool isLocal
    required property bool isPlaying

    property string rowNumber: (root.index + 1).toString()
    property var alpha: PlaylistSettings.overlayVideo ? 0.6 : 1

    implicitWidth: ListView.view.width
    padding: 0
    highlighted: root.isPlaying

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

    contentItem: Kirigami.IconTitleSubtitle {
        icon.name: root.isPlaying ? "media-playback-start" : ""
        icon.color: color
        title: root.mainText()
        subtitle: root.duration
        color: root.hovered || root.highlighted ? Kirigami.Theme.highlightedTextColor : Kirigami.Theme.textColor
        ToolTip.text: title
        ToolTip.visible: root.hovered
    }

    onClicked: {
        if (PlaylistSettings.openWithSingleClick) {
            root.m_mpv.playlistProxyModel.setPlayingItem(index)
        }
    }

    onDoubleClicked: {
        if (!PlaylistSettings.openWithSingleClick) {
            root.m_mpv.playlistProxyModel.setPlayingItem(index)
        }
    }

    function mainText() : string {
        const rowNumber = "%1. ".arg(pad(root.rowNumber, root.ListView.view.count.toString().length))

        if(PlaylistSettings.showRowNumber) {
            return rowNumber + (PlaylistSettings.showMediaTitle ? root.title : root.name)
        }
        return (PlaylistSettings.showMediaTitle ? root.title : root.name)
    }

    function pad(number: string, length: int) : string {
        while (number.length < length) {
            number = "0" + number
        }
        return number
    }
}
