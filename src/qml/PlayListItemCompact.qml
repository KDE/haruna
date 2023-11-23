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
import Haruna.Components as HC

ItemDelegate {
    id: root

    property bool isLocal: model.isLocal
    property string rowNumber: (index + 1).toString()
    property var alpha: PlaylistSettings.overlayVideo ? 0.6 : 1

    padding: 0
    contentItem: Kirigami.IconTitleSubtitle {
        icon: model.isPlaying ? "media-playback-start" : ""
        title: mainText()
        subtitle: model.duration
    }

    onDoubleClicked: {
        mpv.playlistProxyModel.setPlayingItem(index)
        mpv.loadFile(path)
        mpv.pause = false
    }

    ToolTip {
        text: (PlaylistSettings.showMediaTitle ? model.title : model.name)
        visible: root.containsMouse
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
