import QtQuick
import QtQuick.Controls

import org.kde.haruna

ToolButton {
    id: root

    required property string toolTipText
    property alias toolTipWidth: toolTip.width
    property alias toolTipHeight: toolTip.height

    icon.name: "documentinfo"
    checkable: true
    checked: false

    ToolTip {
        id: toolTip

        y: root.height
        visible: root.checked

        delay: 0
        timeout: -1
        closePolicy: Popup.NoAutoClose

        contentItem: ScrollView {
            anchors.fill: parent
            anchors.rightMargin: 0

            TextArea {
                text: root.toolTipText
                background: Rectangle {
                    color: "transparent"
                }
                readOnly: true
                textFormat: Text.RichText
                wrapMode: Text.WordWrap
                selectByMouse: true
                padding: 15
                onLinkActivated: function(link) {
                    Qt.openUrlExternally(link)
                }
                onHoveredLinkChanged: hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
            }
        }
    }
}
