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

    property bool isLocal: model.isLocal
    property string rowNumber: (index + 1).toString()
    property var alpha: PlaylistSettings.overlayVideo ? 0.6 : 1

    implicitWidth: ListView.view.width
    padding: 0
    highlighted: model.isPlaying

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

    contentItem: Kirigami.IconTitleSubtitle {
        icon.name: model.isPlaying ? "media-playback-start" : ""
        icon.color: color
        title: mainText()
        subtitle: model.duration
        color: root.hovered || root.highlighted ? Kirigami.Theme.highlightedTextColor : Kirigami.Theme.textColor
        ToolTip.text: title
        ToolTip.visible: root.hovered
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

    function mainText() {
        const rowNumber = pad(root.rowNumber, playlistView.count.toString().length) + ". "

        if(PlaylistSettings.showRowNumber) {
            return rowNumber + (PlaylistSettings.showMediaTitle ? model.title : model.name)
        }
        return (PlaylistSettings.showMediaTitle ? model.title : model.name)
    }

    function pad(number, length) {
        while (number.length < length)
            number = "0" + number;
        return number;
    }
}
