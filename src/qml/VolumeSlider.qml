/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.13
import QtQuick.Controls 2.13
import QtQml 2.13
import Qt5Compat.GraphicalEffects
import org.kde.kirigami 2.11 as Kirigami

Slider {
    id: root

    from: 0
    to: 100
    implicitWidth: 100
    implicitHeight: 25
    wheelEnabled: true
    stepSize: settings.get("General", "VolumeStep")
    leftPadding: 0
    rightPadding: 0

    handle: Item { visible: false }

    background: Rectangle {
        id: harunaSliderBG
        color: Kirigami.Theme.backgroundColor

        Rectangle {
            width: visualPosition * parent.width
            height: parent.height
            color: Kirigami.Theme.highlightColor
            radius: 0
        }
    }

    Label {
        id: progressBarToolTip
        text: root.value
        anchors.centerIn: root
        color: "#fff"
        layer.enabled: true
        layer.effect: DropShadow { verticalOffset: 1; color: "#111"; radius: 5; spread: 0.3; samples: 17 }
    }

    onPressedChanged: {
        if (!pressed) {
            mpv.setProperty("volume", value.toFixed(0))
        }
    }

    onValueChanged: {

        mpv.setProperty("volume", value.toFixed(0))
        settings.set("General", "Volume", value.toFixed(0))
    }

    Connections {
        target: mpv
        onVolumeChanged: value = mpv.volume
    }
    Connections {
        target: app
        onSettingsChanged: stepSize = settings.get("General", "VolumeStep")
    }
}
