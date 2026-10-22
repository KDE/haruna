/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import org.kde.ki18n

import org.kde.haruna
import org.kde.haruna.utilities
import org.kde.haruna.settings

SettingsBasePage {
    id: root

    ColumnLayout {

        RowLayout {
            // OSD Font Size
            Label {
                text: KI18n.i18nc("@label:spinbox", "OSD font size")
                Layout.alignment: Qt.AlignRight
            }

            SpinBox {
                id: osdFontSize

                editable: true
                from: 0
                to: 100
                value: OsdSettings.osdFontSize
                onValueModified: {
                    OsdSettings.osdFontSize = osdFontSize.value
                    OsdSettings.save()
                }
            }
        }

        SettingsHeader {
            text: KI18n.i18nc("@title", "Choose when to show osd")
            Layout.columnSpan: 2
            Layout.fillWidth: true
        }

        Repeater {
            model: OsdUtils.options()
            delegate: CheckBox {
                required property string modelData

                text: OsdUtils.label(modelData)
                checked: OsdSettings[modelData]
                onClicked: {
                    OsdSettings[modelData] = checked
                    OsdSettings.save()
                }
            }
        }
    }
}
