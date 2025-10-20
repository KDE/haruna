/*
 * SPDX-FileCopyrightText: 2022 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami

Popup {
    id: root

    signal submitted(string text)
    property string lastText: ""
    property string placeholderText: ""
    property string buttonText: ""
    property alias warningText: warningMessage.text

    modal: true

    onOpened: {
        textField.forceActiveFocus(Qt.MouseFocusReason)
        textField.selectAll()
    }

    contentItem: RowLayout {
        anchors.fill: parent
        anchors.leftMargin: Kirigami.Units.largeSpacing
        anchors.rightMargin: Kirigami.Units.largeSpacing

        Label {
            id: warningMessage

            visible: text !== ""
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            onLinkActivated: (link) => Qt.openUrlExternally(link)

            Layout.fillWidth: true
        }

        TextField {
            id: textField

            text: root.lastText
            placeholderText: root.placeholderText
            visible: root.warningText === ""

            Layout.fillWidth: true

            Keys.onPressed: function(event) {
                switch(event.key) {
                case Qt.Key_Enter:
                case Qt.Key_Return:
                    submitButton.clicked()
                    return;
                case Qt.Key_Escape:
                    root.close()
                    return;
                }
            }
        }

        Button {
            id: submitButton

            visible: root.warningText === ""
            text: root.buttonText

            onClicked: {
                root.submitted(textField.text)
                root.close()
                textField.clear()
            }
        }
    }
}
