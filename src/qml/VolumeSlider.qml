/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQml 2.12
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Shapes 1.12
import QtGraphicalEffects 1.12

import org.kde.kirigami 2.11 as Kirigami
import org.kde.haruna 1.0

Slider {
    id: root

    from: 0
    to: 100
    value: mpv.volume
    implicitWidth: 100
    implicitHeight: 25
    wheelEnabled: true
    stepSize: GeneralSettings.volumeStep
    leftPadding: 0
    rightPadding: 0

    handle: Item { visible: false }

    background: Rectangle {
        id: harunaSliderBG
        color: Kirigami.Theme.alternateBackgroundColor

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

    onValueChanged: {
        mpv.volume = value.toFixed(0)
        GeneralSettings.volume = value.toFixed(0)
        GeneralSettings.save()
    }

}
