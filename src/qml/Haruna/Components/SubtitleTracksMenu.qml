/*
 * SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls

Menu {
    id: root

    required property var model
    required property bool isPrimarySubtitleMenu

    Repeater {
        model: root.model
        delegate: MenuItem {
            enabled: isPrimarySubtitleMenu
                     ? model.id !== mpv.secondarySubtitleId || model.id === 0
                     : model.id !== mpv.subtitleId || model.id === 0
            checkable: true
            checked: isPrimarySubtitleMenu
                     ? model.id === mpv.subtitleId
                     : model.id === mpv.secondarySubtitleId
            text: model.text
            onTriggered: {
                if (isPrimarySubtitleMenu) {
                    mpv.subtitleId = model.id
                } else {
                    mpv.secondarySubtitleId = model.id
                }
            }
        }
    }
}
