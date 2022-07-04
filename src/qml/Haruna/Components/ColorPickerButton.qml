import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.3

import org.kde.kirigami 2.11 as Kirigami

Rectangle {
    id: root

    // using onColorChanged causes a binding loop
    signal colorChosen(string color)

    implicitWidth: Kirigami.Units.gridUnit * 1.5
    implicitHeight: Kirigami.Units.gridUnit * 1.5

    onColorChosen: color = colorPicker.color

    MouseArea {
        id: ma

        anchors.fill: parent
        hoverEnabled: true
        onClicked: colorPicker.open()
        cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor
    }

    ColorDialog {
        id: colorPicker

        title: qsTr("Select a color")
        onAccepted: colorChosen(colorPicker.color)
        color: root.color
    }
}
