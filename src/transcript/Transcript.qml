/*
 * SPDX-FileCopyrightText: 2026 George Florea Bănuș <georgefb899@gmail.com>
 * SPDX-FileCopyrightText: 2026 Muhammet Sadık Uğursoy <sadikugursoy@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.kde.ki18n
import org.kde.kirigami as Kirigami

import org.kde.haruna
import org.kde.haruna.settings

ResizeablePage {
    id: root

    edge: PlaylistSettings.position === "right" ? Qt.LeftEdge : Qt.RightEdge
    customWidth: 380
    width: limitWidth(customWidth * fsScale)

    function limitWidth(pWidth) {
        return Math.min(Math.max(pWidth, 260), mainWindowWidth - 50)
    }

    onResize: function (delta) {
        // invert the drag delta when the playlist is anchored to the right
        // dragging left (pX is negative) expands a right-aligned playlist, but shrinks a left-aligned one
        const widthDelta = root.edge === Qt.RightEdge ? delta * -1 : delta;
        root.customWidth = root.limitWidth(root.customWidth + widthDelta)
    }

    header: ToolBar {
        id: toolbar

        width: parent.width

        RowLayout {
            anchors.fill: parent

            ToolButton {
                icon.name: "add-subtitle"
                icon.width: root.buttonSize
                icon.height: root.buttonSize
                focusPolicy: Qt.NoFocus
                enabled: false
                opacity: enabled ? 1.0 : 0.6
                display: AbstractButton.IconOnly

                ToolTip.text: KI18n.i18nc("@info:tooltip", "Select subtitle")
            }

            Kirigami.SearchField {
                Layout.fillWidth: true
            }
        }
    }

    pageContent: [
        ScrollView {
            id: transcriptScrollView

            z: 1
            anchors.fill: parent
            anchors {
                leftMargin: root.pageEdgeBorder.width
                rightMargin: root.pageEdgeBorder.width
            }

            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

            ListView {
                id: transcriptView
                model: 0
                delegate: Item {}
            }
        }
    ]
}
