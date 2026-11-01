/*
 * SPDX-FileCopyrightText: 2026 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import org.kde.ki18n

import org.kde.haruna

SettingsBasePage {
    id: root

    GridLayout {
        id: content

        columns: 2

        Label {
            text: KI18n.i18nc("@label", "Clear recent files")
            Layout.alignment: Qt.AlignRight
        }
        RowLayout {
            Layout.fillWidth: true

            Button {
                text: KI18n.i18nc("@action:button:", "Clear")
                onClicked: Models.recentFilesModel.deleteEntries()
            }
            ToolTipButton {
                toolTipText: KI18n.i18nc("@info:tooltip", "Deletes all recent files saved in the database")
            }
        }

        Label {
            text: KI18n.i18nc("@label", "Clear metadata")
            Layout.alignment: Qt.AlignRight
        }
        RowLayout {
            Layout.fillWidth: true

            Button {
                text: KI18n.i18nc("@action:button:", "Clear")
                onClicked: Database.deleteAllMetadata()
            }
            ToolTipButton {
                toolTipText: KI18n.i18nc("@info:tooltip", "Deletes all metadata saved in the database")
            }
        }

        Label {
            text: KI18n.i18nc("@label", "Clear playback positions")
            Layout.alignment: Qt.AlignRight
        }
        RowLayout {
            Layout.fillWidth: true

            Button {
                text: KI18n.i18nc("@action:button:", "Clear")
                onClicked: Database.deletePlaybackPositions()
            }
            ToolTipButton {
                toolTipText: KI18n.i18nc("@info:tooltip", "Deletes all playback positions saved in the database")
            }
        }
    }
}
