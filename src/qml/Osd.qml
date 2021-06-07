/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.12
import QtQuick.Controls 2.12

import org.kde.kirigami 2.11 as Kirigami
import org.kde.haruna 1.0

Item {
    id: root

    property alias label: label

    Label {
        id: label
        x: 10
        y: 10
        visible: false
        color: Kirigami.Theme.textColor
        background: Rectangle {
            color: Kirigami.Theme.backgroundColor
        }
        padding: 5
        font.pointSize: parseInt(GeneralSettings.osdFontSize)
    }

    Timer {
        id: timer
        running: false
        repeat: false
        interval: 3000

        onTriggered: {
            label.visible = false
        }
    }

    function message(text) {
        const osdFontSize = parseInt(GeneralSettings.osdFontSize)
        label.text = text
        if (osdFontSize === 0) {
            return;
        }

        if(label.visible) {
            timer.restart()
        } else {
            timer.start()
        }
        label.visible = true
    }

}
