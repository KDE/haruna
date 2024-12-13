/*
 * SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls

import org.kde.haruna

Menu {
    id: root

    required property MpvVideo m_mpv

    property TracksModel model: m_mpv.audioTracksModel

    enabled: root.m_mpv.audioTracksModel.rowCount > 0

    Repeater {
        model: root.model
        delegate: MenuItem {
            id: delegate

            required property int trackId
            required property string displayText
            required property string language
            required property string title
            required property string codec

            checkable: true
            checked: delegate.trackId === root.m_mpv.audioId
            text: delegate.displayText
            onTriggered: {
                root.m_mpv.audioId = delegate.trackId
            }
        }
    }
}
