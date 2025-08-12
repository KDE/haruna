/*
 * SPDX-FileCopyrightText: 2021 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

RowLayout {
    id: root

    property int value: 0
    signal sliderValueChanged(int value)

    Slider {
        id: slider

        from: -100
        to: 100
        value: root.value
        wheelEnabled: true
        stepSize: 1
        onValueChanged: root.sliderValueChanged(value.toFixed(0))

        Layout.fillWidth: true

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.MiddleButton
            onClicked: slider.value = 0
        }

        Component.onCompleted: background.activeControl = ""
    }

    Label {
        text: slider.value
        horizontalAlignment: Qt.AlignHCenter
        Layout.preferredWidth: 40
    }
}
