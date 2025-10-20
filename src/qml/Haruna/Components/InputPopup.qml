/*
 * SPDX-FileCopyrightText: 2022 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

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

    RowLayout {
        anchors.fill: parent

        Label {
            id: warningMessage

            visible: text !== ""
            onLinkActivated: (link) => Qt.openUrlExternally(link)
        }

        TextField {
            id: textField

            text: root.lastText
            placeholderText: root.placeholderText
            visible: root.warningText === ""
            Layout.preferredWidth: 400
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
