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

    Repeater {
        model: root.model
        delegate: MenuItem {
            id: delegate

            required property string displayText
            required property string language
            required property string title
            required property string codec
            required property string trackId

            checkable: true
            checked: delegate.trackId === mpv.audioId
            text: delegate.displayText
            onTriggered: {
                mpv.audioId = delegate.trackId
            }
        }
    }
}
