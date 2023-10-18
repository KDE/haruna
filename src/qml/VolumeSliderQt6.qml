/*
 * SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Shapes 1.15
import Qt5Compat.GraphicalEffects

import org.kde.kirigami 2.20 as Kirigami
import org.kde.haruna 1.0

Slider {
    id: root

    from: 0
    to: mpv.volumeMax
    value: mpv.volume
    implicitWidth: 100
    implicitHeight: 25
    wheelEnabled: true
    stepSize: GeneralSettings.volumeStep
    leftPadding: 0
    rightPadding: 0

    handle: Item { visible: false }
    background: Rectangle {
        color: Kirigami.Theme.alternateBackgroundColor
        scale: root.mirrored ? -1 : 1

        Rectangle {
            width: root.position * parent.width
            height: parent.height
            color: Kirigami.Theme.highlightColor
        }
    }

    onPressedChanged: {
        mpv.volume = value.toFixed(0)
    }

    onValueChanged: {
        GeneralSettings.volume = value.toFixed(0)
        GeneralSettings.save()
        osd.message(i18nc("@info:tooltip", "Volume: %1", GeneralSettings.volume))
    }

    Label {
        id: progressBarToolTip
        text: root.value
        anchors.centerIn: root
        color: "#fff"
        layer.enabled: true
        layer.effect: DropShadow { verticalOffset: 1; color: "#111"; radius: 5; spread: 0.3; samples: 17 }
    }
}
