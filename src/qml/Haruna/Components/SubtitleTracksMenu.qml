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
            id: delegate

            required property string displayText
            required property string language
            required property string title
            required property string codec
            required property string trackId

            enabled: isPrimarySubtitleMenu
                     ? delegate.trackId !== mpv.secondarySubtitleId || delegate.trackId === 0
                     : delegate.trackId !== mpv.subtitleId || delegate.trackId === 0
            checkable: true
            checked: isPrimarySubtitleMenu
                     ? delegate.trackId === mpv.subtitleId
                     : delegate.trackId === mpv.secondarySubtitleId
            text: delegate.displayText
            onTriggered: {
                if (isPrimarySubtitleMenu) {
                    mpv.subtitleId = delegate.trackId
                } else {
                    mpv.secondarySubtitleId = delegate.trackId
                }
            }
        }
    }
}
