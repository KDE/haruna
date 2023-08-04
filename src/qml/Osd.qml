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

    required property int maxWidth
    property bool active: false
    property alias label: label

    Label {
        id: label

        x: Kirigami.Units.largeSpacing
        y: Kirigami.Units.largeSpacing
        width: textMetrics.advanceWidth > maxWidth ? maxWidth : textMetrics.advanceWidth + padding * 2
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

    function message(text) {
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
