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
    required property bool isPrimarySubtitleMenu

    property TracksModel model: m_mpv.subtitleTracksModel

    enabled: m_mpv.subtitleTracksModel.rowCount() > 1

    Repeater {
        model: root.model
        delegate: MenuItem {
            id: delegate

            required property string displayText
            required property string language
            required property string title
            required property string codec
            required property string trackId

            enabled: root.isPrimarySubtitleMenu
                     ? delegate.trackId !== root.m_mpv.secondarySubtitleId || delegate.trackId === 0
                     : delegate.trackId !== root.m_mpv.subtitleId || delegate.trackId === 0
            checkable: true
            checked: root.isPrimarySubtitleMenu
                     ? delegate.trackId === root.m_mpv.subtitleId
                     : delegate.trackId === root.m_mpv.secondarySubtitleId
            text: delegate.displayText
            onTriggered: {
                if (root.isPrimarySubtitleMenu) {
                    root.m_mpv.subtitleId = delegate.trackId
                } else {
                    root.m_mpv.secondarySubtitleId = delegate.trackId
                }
            }
        }
    }
}
