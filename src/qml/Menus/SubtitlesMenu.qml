/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQml 2.13
import QtQuick 2.13
import QtQuick.Controls 2.13

Menu {
    id: root

    title: qsTr("&Subtitles")

    Menu {
        id: primarySubtitleMenu

        title: qsTr("Primary Subtitle")
        onOpened: primaryMenuItems.model = mpv.subtitleTracksModel()

        TrackMenuItems {
            id: primaryMenuItems

            menu: primarySubtitleMenu
            isFirst: true
            onSubtitleChanged: function(id, index) {
                mpv.setSubtitle(id)
                mpv.subtitleTracksModel().updateFirstTrack(index)
            }
        }
    }

    Menu {
        id: secondarySubtitleMenu

        title: qsTr("Secondary Subtitle")
        onOpened: secondaryMenuItems.model = mpv.subtitleTracksModel()

        TrackMenuItems {
            id: secondaryMenuItems

            menu: secondarySubtitleMenu
            isFirst: false
            onSubtitleChanged: function(id, index) {
                mpv.setSecondarySubtitle(id)
                mpv.subtitleTracksModel().updateSecondTrack(index)
            }
        }
    }

    MenuSeparator {}

    MenuItem { action: actions["subtitleQuickenAction"] }
    MenuItem { action: actions["subtitleDelayAction"] }
    MenuItem { action: actions["subtitleToggleAction"] }
}
