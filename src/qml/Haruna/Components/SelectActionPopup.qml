/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kitemmodels as KItemModels

import org.kde.haruna

Kirigami.SearchDialog {
    id: root

    signal actionSelected(string actionName)

    onActionSelected: function(actionName) {
        close()
    }

    emptyText: i18nc("@info:placeholder", "No action found.")
    model: KItemModels.KSortFilterProxyModel {
        id: sortModel

        sourceModel: ActionsModel {}
        filterRoleName: "searchString"
        filterCaseSensitivity: Qt.CaseInsensitive
        sortRoleName: "actionText"
        sortOrder: Qt.AscendingOrder
    }

    onTextChanged: {
        sortModel.filterString = text
    }

    delegate: Loader {
        id: actionDelegateLoader

        required property int index
        required property string actionName
        required property string actionText
        required property string actionShortcut
        required property string actionDescription
        required property string actionType

        active: true
        asynchronous: true
        width: ListView.view.width
        height: actionDescription === "" ? Kirigami.Units.gridUnit * 2 : Kirigami.Units.gridUnit * 3

        Keys.forwardTo: [item]

        sourceComponent: ItemDelegate {
            id: actionDelegate

            highlighted: actionDelegateLoader.ListView.view.currentIndex === actionDelegateLoader.index

            onClicked: root.actionSelected(actionDelegateLoader.actionName)
            Keys.onEnterPressed: root.actionSelected(actionDelegateLoader.actionName)
            Keys.onReturnPressed: root.actionSelected(actionDelegateLoader.actionName)

            contentItem: RowLayout {

                ColumnLayout {
                    Layout.fillWidth: true

                    Label {
                        id: shortcutTextLabel

                        text: actionDelegateLoader.actionText
                        Layout.fillWidth: true
                    }
                    Label {
                        text: actionDelegateLoader.actionDescription
                        visible: text !== ""
                        opacity: 0.9
                        font.pointSize: shortcutTextLabel.font.pointSize - 1
                    }
                }

                Repeater {
                    model: actionDelegateLoader.actionShortcut.split("+")
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
}
