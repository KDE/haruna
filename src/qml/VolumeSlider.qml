/*
 * SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects

import org.kde.kirigami as Kirigami
import org.kde.haruna
import org.kde.haruna.settings

Slider {
    id: root

    required property MpvItem m_mpv

    property alias label: volumeLabel

    from: 0
    to: root.m_mpv.volumeMax
    value: root.m_mpv.volume
    implicitWidth: 100
    implicitHeight: 25
    wheelEnabled: true
    stepSize: AudioSettings.volumeStep
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
        root.m_mpv.volume = value.toFixed(0)
    }

    onValueChanged: {
        if (!root.m_mpv.isReady) {
            return
        }

        root.m_mpv.volume = value.toFixed(0)
    }

    Label {
        id: volumeLabel
        text: root.value
        anchors.centerIn: root
        color: "#fff"
        layer.enabled: true
        layer.effect: DropShadow { verticalOffset: 1; color: "#111"; radius: 5; spread: 0.3; samples: 17 }
    }
}
