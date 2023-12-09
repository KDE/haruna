/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQml
import QtQuick
import QtQuick.Controls

import "../Haruna/Components"

Menu {
    id: root

    title: i18nc("@title:menu", "&Audio")

    AudioTracksMenu {
        id: audioMenu

        y: parent.height
        title: i18nc("@title:menu", "&Track")
        model: mpv.audioTracksModel
    }

    MenuSeparator {}

    MenuItem { action: appActions.muteAction }
    MenuItem { action: appActions.volumeUpAction }
    MenuItem { action: appActions.volumeDownAction }
    MenuItem { action: appActions.audioCycleUpAction }
    MenuItem { action: appActions.audioCycleDownAction }
}
