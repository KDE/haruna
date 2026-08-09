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

    property TranscriptModel transcriptModel: TranscriptModel {}
    property int selectedTrackId: -1

    edge: PlaylistSettings.position === "right" ? Qt.LeftEdge : Qt.RightEdge
    customWidth: 380
    width: limitWidth(customWidth * fsScale)

    function limitWidth(pWidth) {
        return Math.min(Math.max(pWidth, 260), mainWindowWidth - 50)
    }

    function loadCurrentSubtitle() {
        // Load displayed subtitle until user explicitly selects a subtitle to load.
        let selectedTrackId = root.selectedTrackId
        if (selectedTrackId === -1) {
            selectedTrackId = root.m_mpv.subtitleId
        }

        const subtitleTracksModel = root.m_mpv.subtitleTracksModel
        const trackRow = subtitleTracksModel.findTrackRow(selectedTrackId)
        const modelIndex = subtitleTracksModel.index(trackRow, 0)

        if (!modelIndex) {
            return
        }
        const externalPath = subtitleTracksModel.data(modelIndex, TracksModel.Roles.ExternalPathRole)
        const streamIndex = subtitleTracksModel.data(modelIndex, TracksModel.Roles.StreamIndexRole)

        if (externalPath && externalPath.toString() !== "") {
            root.transcriptModel.loadSubtitle(externalPath, streamIndex)
        } else {
            root.transcriptModel.loadSubtitle(root.m_mpv.currentUrl, streamIndex)
        }
        subtitleMenu.loadedTrackId = selectedTrackId
    }

    onResize: function (delta) {
        // invert the drag delta when the playlist is anchored to the right
        // dragging left (pX is negative) expands a right-aligned playlist, but shrinks a left-aligned one
        const widthDelta = root.edge === Qt.RightEdge ? delta * -1 : delta;
        root.customWidth = root.limitWidth(root.customWidth + widthDelta)
    }

    onStateChanged: {
        if (root.state === "visible") {
            root.loadCurrentSubtitle()
        }
        if (root.state === "hidden") {
            root.transcriptModel.clearSubtitle()
            subtitleMenu.loadedTrackId = 0
        }
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
                enabled: root.m_mpv.subtitleTracksModel.rowCount > 1
                opacity: enabled ? 1.0 : 0.6
                display: AbstractButton.IconOnly

                ToolTip.text: KI18n.i18nc("@info:tooltip", "Select subtitle")

                onReleased: {
                    subtitleMenu.visible = !subtitleMenu.visible
                }

                Menu {
                    id: subtitleMenu

                    // Tracks current loaded and displayed trackId
                    property int loadedTrackId: 0

                    y: parent.height

                    Instantiator {
                        model: root.m_mpv.subtitleTracksModel

                        delegate: MenuItem {
                            id: delegate

                            required property int trackId
                            required property string displayText
                            required property int streamIndex
                            required property url externalPath

                            autoExclusive: true
                            checkable: true
                            checked: delegate.trackId === subtitleMenu.loadedTrackId
                            text: delegate.displayText

                            onTriggered: {
                                root.selectedTrackId = delegate.trackId
                                if (delegate.externalPath && delegate.externalPath.toString() !== "") {
                                    root.transcriptModel.loadSubtitle(delegate.externalPath, delegate.streamIndex)
                                } else {
                                    root.transcriptModel.loadSubtitle(root.m_mpv.currentUrl, delegate.streamIndex)
                                }
                            }
                        }

                        onObjectAdded: function (index, object) {
                            subtitleMenu.insertItem(index, object)
                        }

                        onObjectRemoved: function (index, object) {
                            subtitleMenu.removeItem(object)
                        }
                    }
                }
            }

            Kirigami.SearchField {
                Layout.fillWidth: true
            }
        }
    }

    pageContent: [
        ScrollView {
            id: transcriptScrollView

            z: 20
            anchors.fill: parent
            anchors {
                leftMargin: root.pageEdgeBorder.width
                rightMargin: root.pageEdgeBorder.width
            }

            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

            ListView {
                id: transcriptView
                model: root.transcriptModel
                reuseItems: true
                spacing: 1

                delegate: Item {}
            }
        }
    ]
}
