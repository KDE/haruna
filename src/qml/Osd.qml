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

Item {
    id: root

    required property int maxWidth

    property bool active: false

    Label {
        id: label

        property int textWidth: Math.ceil(textMetrics.advanceWidth) + padding * 2

        x: Kirigami.Units.largeSpacing
        y: Kirigami.Units.largeSpacing
        width: textWidth > root.maxWidth ? root.maxWidth : undefined
        visible: false
        background: Rectangle {
            color: Kirigami.Theme.backgroundColor
        }
        padding: Kirigami.Units.largeSpacing
        font.pointSize: parseInt(GeneralSettings.osdFontSize)
        maximumLineCount: 5
        wrapMode: Text.WrapAnywhere

        TextMetrics {
            id: textMetrics

            font: label.font
            text: label.text
        }
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

    function message(text: string) : void {
        if (!root.active) {
            return
        }

        if (text === "" || text === null || text === undefined) {
            return
        }

        const osdFontSize = parseInt(GeneralSettings.osdFontSize)
        if (osdFontSize === 0) {
            return;
        }

        timer.restart()
        label.text = text
        label.visible = true
    }

}
