/*
 * SPDX-FileCopyrightText: 2022 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.kde.haruna

Popup {
    id: root

    signal urlOpened(string url)
    property string lastUrl: ""
    property string placeholderText: ""
    property string buttonText: ""

    modal: true

    onOpened: {
        openUrlTextField.forceActiveFocus(Qt.MouseFocusReason)
        openUrlTextField.selectAll()
    }

    YouTube {
        id: youtube
    }

    RowLayout {
        anchors.fill: parent

        Label {
            text: i18nc("@info", "Neither <a href=\"https://github.com/yt-dlp/yt-dlp\">yt-dlp</a> nor <a href=\"https://github.com/ytdl-org/youtube-dl\">youtube-dl</a> was found.")
            visible: !youtube.hasYoutubeDl()
            onLinkActivated: (link) => Qt.openUrlExternally(link)
        }

        TextField {
            id: openUrlTextField

            text: root.lastUrl
            placeholderText: root.placeholderText
            visible: youtube.hasYoutubeDl()
            Layout.preferredWidth: 400
            Layout.fillWidth: true

            Keys.onPressed: function(event) {
                switch(event.key) {
                case Qt.Key_Enter:
                case Qt.Key_Return:
                    openUrlButton.clicked()
                    return;
                case Qt.Key_Escape:
                    root.close()
                    return;
                }
            }
        }

        Button {
            id: openUrlButton

            visible: youtube.hasYoutubeDl()
            text: root.buttonText

            onClicked: {
                root.urlOpened(openUrlTextField.text)
                root.close()
                openUrlTextField.clear()
            }
        }
    }
}
