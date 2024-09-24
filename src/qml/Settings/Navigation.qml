/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import org.kde.kirigami 2.11 as Kirigami

Flickable {
    id: root

    clip: true
    ScrollBar.vertical: ScrollBar { id: scrollbar }

    ListModel {
        id: settingsPagesModel
        ListElement {
            name: "General"
            iconName: "configure"
        }
        ListElement {
            name: "Playback"
            iconName: "media-playback-start"
        }
        ListElement {
            name: "Video"
            iconName: "video-x-generic"
        }
        ListElement {
            name: "Audio"
            iconName: "audio-speakers-symbolic"
        }
        ListElement {
            name: "Subtitles"
            iconName: "media-view-subtitles-symbolic"
        }
        ListElement {
            name: "Playlist"
            iconName: "view-media-playlist"
        }
        ListElement {
            name: "Mouse"
            iconName: "input-mouse"
        }
        ListElement {
            name: "Color Adjustments"
            iconName: "color-management"
        }
        ListElement {
            name: "Shortcuts"
            iconName: "configure-shortcuts"
        }
    }

    ListView {
        id: settingsPagesList

        anchors.fill: parent
        model: settingsPagesModel
        delegate: ItemDelegate {
            text: qsTr(name)
            icon.name: iconName
            width: parent.width
            onClicked: {
                let activeComponent;
                switch (name) {
                case "General":
                    activeComponent = generalSettings
                    break;
                case "Playback":
                    activeComponent = playbackSettings
                    break;
                case "Video":
                    activeComponent = videoSettings
                    break;
                case "Audio":
                    activeComponent = audioSettings
                    break;
                case "Subtitles":
                    activeComponent = subtitlesSettings
                    break;
                case "Playlist":
                    activeComponent = playlistSettings
                    break;
                case "Mouse":
                    activeComponent = mouseSettings
                    break;
                case "Color Adjustments":
                    activeComponent = colorAdjustmentsSettings
                    break;
                case "Shortcuts":
                    // set visible true so since here we open another window
                    // and if visible is not true the settings page
                    // will be hidden until the opened window is closed
                    settingsPageLoader.item.visible = true
                    actions.configureShortcutsAction.trigger()
                    return;
                }
                settingsPageLoader.item.visible = false
                settingsPageLoader.sourceComponent = activeComponent
                settingsPageLoader.item.visible = true
            }
        }
    }
}
