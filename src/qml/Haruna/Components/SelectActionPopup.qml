/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.kde.ki18n
import org.kde.kirigami as Kirigami
import org.kde.kitemmodels as KItemModels

import org.kde.haruna

Kirigami.SearchDialog {
    id: root

    property string actionName: ""
    signal actionSelected(string actionName)

    onActionSelected: function(actionName) {
        root.actionName = actionName
        close()
    }

    emptyText: KI18n.i18nc("@info:placeholder", "No action found.")
    model: KItemModels.KSortFilterProxyModel {
        id: sortModel

        sourceModel: Models.actionsModel
        filterRoleName: "searchString"
        filterCaseSensitivity: Qt.CaseInsensitive
        sortRoleName: "actionText"
        sortOrder: Qt.AscendingOrder
    }

    onTextChanged: {
        sortModel.filterString = text
    }

    delegate: ItemDelegate {
        id: actionDelegate

        required property int index
        required property string actionName
        required property string actionText
        required property string actionShortcut
        required property string actionDescription
        required property string actionType

        width: ListView.view.width
        onClicked: root.actionSelected(actionName)
        Keys.onEnterPressed: root.actionSelected(actionName)
        Keys.onReturnPressed: root.actionSelected(actionName)

        contentItem: RowLayout {

            ColumnLayout {
                Layout.fillWidth: true

                Label {
                    id: shortcutTextLabel

                    text: actionDelegate.actionText
                    Layout.fillWidth: true
                }
                Label {
                    text: actionDelegate.actionDescription
                    visible: text !== ""
                    opacity: 0.9
                    font.pointSize: shortcutTextLabel.font.pointSize - 1
                }
            }

            Repeater {
                model: actionDelegate.actionShortcut === "" ? 0 : actionDelegate.actionShortcut.split("+")
                delegate: Kirigami.Chip {
                    id: shortcutDelgate

                    required property string modelData

                    Layout.preferredWidth: actionShortcutLabel.width + Kirigami.Units.largeSpacing
                    Layout.preferredHeight: actionShortcutLabel.height + Kirigami.Units.largeSpacing

                    closable: false
                    checkable: false

                    Label {
                        id: actionShortcutLabel

                        text: shortcutDelgate.modelData
                        anchors.centerIn: parent
                    }
                }
            }
        }
    }
}
