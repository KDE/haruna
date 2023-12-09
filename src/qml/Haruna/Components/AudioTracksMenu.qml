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
            checkable: true
            checked: model.id === mpv.audioId
            text: model.text
            onTriggered: {
                mpv.audioId = model.id
            }
        }
    }
}
