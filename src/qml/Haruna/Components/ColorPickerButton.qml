/*
 * SPDX-FileCopyrightText: 2022 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.3

import org.kde.kirigami 2.11 as Kirigami

Rectangle {
    id: root

    // using onColorChanged causes a binding loop
    signal colorChosen(string color)

    implicitWidth: Math.ceil(Kirigami.Units.gridUnit * 1.7)
    implicitHeight: Math.ceil(Kirigami.Units.gridUnit * 1.7)
    border.width: 1
    border.color: ma.containsMouse
                  ? Kirigami.Theme.highlightColor
                  : Kirigami.Theme.alternateBackgroundColor

    onColorChosen: {
        root.color = colorPicker.color
    }

    MouseArea {
        id: ma

        anchors.fill: parent
        hoverEnabled: true
        onClicked: colorPicker.open()
        cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor
    }

    ToolTip {
        text: i18nc("@info:tooltip", "Select a color")
        visible: ma.containsMouse
    }

    ColorDialog {
        id: colorPicker

        title: qsTr("Select a color")
        onAccepted: colorChosen(colorPicker.color)
        color: root.color
    }

    Connections {
        target: window
        onClosing: colorPicker.close()
    }
}
