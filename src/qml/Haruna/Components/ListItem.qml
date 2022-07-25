/*
 *  SPDX-FileCopyrightText: 2010 Marco Martin <notmart@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.8
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.0 as QQC2
import org.kde.kirigami 2.12

/**
 * A BasicListItem provides a simple list item design that can handle the
 * most common list item usecases.
 *
 * @image html BasicListItemTypes.svg "The styles of the BasicListItem. From left to right top to bottom: light icon + title + subtitle, dark icon + title + subtitle, light icon + label, dark icon + label, light label, dark label." width=50%
 */
AbstractListItem {
    id: listItem

    /**
     * @brief This property holds label of this list item.
     *
     * If a subtitle is provided, the label will behave as a title and will be styled
     * accordingly. Every list item should have a label.
     *
     * @property string label
     */
    property alias label: listItem.text

    /**
     * @brief This property holds an optional subtitle that can appear under the label.
     *
     * @since 5.70
     * @since org.kde.kirigami 2.12
     */
    property alias subtitle: subtitleItem.text

    /**
     * This property holds an item that will be displayed before the title and subtitle.
     *
     * @note The leading item is allowed to expand infinitely horizontally, and should be bounded by the user.
     *
     * @since org.kde.kirigami 2.15
     */
    property Item leading
    onLeadingChanged: {
        if (!!listItem.leading) {
            listItem.leading.parent = contItem
            listItem.leading.anchors.left = listItem.leading.parent.left
            listItem.leading.anchors.top = listItem.leadingFillVertically ? listItem.leading.parent.top : undefined
            listItem.leading.anchors.bottom = listItem.leadingFillVertically ? listItem.leading.parent.bottom : undefined
            listItem.leading.anchors.verticalCenter = listItem.leadingFillVertically ? undefined : listItem.leading.parent.verticalCenter
            layout.anchors.left = listItem.leading.right
            layout.anchors.leftMargin = Qt.binding(function() { return listItem.leadingPadding })
        } else {
            layout.anchors.left = contentItem.left
            layout.anchors.leftMargin = 0
        }
    }

    /**
     * This property holds a the padding after the leading item.
     *
     * @since org.kde.kirigami 2.15
     */
    property real leadingPadding: Units.largeSpacing

    /**
     * TODO KF6: remove this property and instead implement leading and trailing
     * item positioning in such a way that they fill vertically, but a fixed
     * height can be manually specified without needing to wrap it in an Item
     *
     * leadingFillVertically: bool
     * Whether or not to stretch the leading item to fit all available vertical space.
     * Defaults to true. If false, you will be responsible for setting a height for the
     * item or ensuring that its default height works.
     *
     * @since 5.83
     * @since org.kde.kirigami 2.15
     */
    property bool leadingFillVertically: true

    /**
     * This property holds an item that will be displayed after the title and subtitle. Note that the
     * trailing item is allowed to expand infinitely horizontally, and should be bounded by the user.
     *
     * @since org.kde.kirigami 2.15
     */

    property Item trailing
    onTrailingChanged: {
        if (!!listItem.trailing) {
            listItem.trailing.parent = contItem
            listItem.trailing.anchors.right = listItem.trailing.parent.right
            listItem.trailing.anchors.top = listItem.trailingFillVertically ? listItem.trailing.parent.top : undefined
            listItem.trailing.anchors.bottom = listItem.trailingFillVertically ? listItem.trailing.parent.bottom : undefined
            listItem.trailing.anchors.verticalCenter = listItem.trailingFillVertically ? undefined : listItem.trailing.parent.verticalCenter
            layout.anchors.right = listItem.trailing.left
            layout.anchors.rightMargin = Qt.binding(function() { return listItem.trailingPadding })
        } else {
            layout.anchors.right = contentItem.right
            layout.anchors.rightMargin = 0
        }
    }

    /**
     * This property holds the padding before the trailing item.
     *
     * @since org.kde.kirigami 2.15
     */
    property real trailingPadding: Units.largeSpacing

    /**
     * TODO KF6: remove this property and instead implement leading and trailing
     * item positioning in such a way that they fill vertically, but a fixed
     * height can be manually specified without needing to wrap it in an Item
     *
     * trailingFillVertically: bool
     * Whether or not to stretch the trailing item to fit all available vertical space.
     * Defaults to true. If false, you will be responsible for setting a height for the
     * item or ensuring that its default height works.
     *
     * @since 5.83
     * @since org.kde.kirigami 2.15
     */
    property bool trailingFillVertically: true

    /**
     * This property holds whether the list item's text (both label and subtitle, if provided) should
     * render in bold.
     *
     * @since 5.71
     * @since org.kde.kirigami 2.13
     */
    property bool bold: false

    /**
     * @code ts
     * interface IconGroup {
     *     name:   string,
     *     source: string,
     *     width:  int,
     *     height: int,
     *     color:  color,
     * }
     *
     * type Icon = string | IconGroup | URL
     * @endcode
     *
     * The icon that will render on this list item.
     *
     * This can either be an icon name, a URL, or an object with the following properties:
     *
     * - `name`: string
     * - `source`: string
     * - `width`: int
     * - `height`: int
     * - `color`: color
     *
     * If the type of the icon is a string containing an icon name, the icon will be looked up from the
     * platform icon theme.
     *
     * Using an icon object allows you to specify more granular attributes of the icon,
     * such as its color and dimensions.
     *
     * If the icon is a URL, the icon will be attempted to be loaded from the
     * given URL.
     */
    property var icon

    /**
     * This property holds the size at which the icon will render. This will not affect icon lookup,
     * unlike the icon group's width and height properties, which will.
     *
     * @property int iconSize
     * @since 2.5
     */
    property alias iconSize: iconItem.size

    /**
     * This property holds the color of the icon.
     *
     * If the icon's original colors should be left intact, set this to the default value, "transparent".
     * Note that this colour will only be applied if the icon can be recoloured, (e.g. you can use Kirigami.Theme.foregroundColor to change the icon's colour.)
     *
     * @since 2.7
     * @property color iconColor
     */
    property alias iconColor: iconItem.color

    /**
     * This property holds whether or not the icon has a "selected" appearance.
     *
     * Can be used to override the icon coloration if the list item's background and
     * text are also being overridden, to ensure that the icon never becomes invisible.
     *
     * @since 5.91
     * @since org.kde.kirigami 2.19
     * @property bool iconSelected
     */
    property alias iconSelected: iconItem.selected

    /**
     * This property holds whether or not to reserve space for the icon, even if there is no icon.
     *
     * @image html BasicListItemReserve.svg "Left: reserveSpaceForIcon: false. Right: reserveSpaceForIcon: true" width=50%
     * @property bool reserveSpaceForIcon
     */
    property alias reserveSpaceForIcon: iconItem.visible

    /**
     * This property holds whether or not the label of the list item should fill width.
     *
     * Setting this to false is useful if you have other items in the list item
     * that should fill width instead of the label.
     *
     * @property bool reserveSpaceForLabel
     */
    property alias reserveSpaceForLabel: labelItem.visible

    /**
     * This property holds whether or not the list item's height should account for
     * the presence of a subtitle, even if one is not present.
     *
     * @since 5.77
     * @since org.kde.kirigami 2.15
     */
    property bool reserveSpaceForSubtitle: false

    /**
     * This property holds the spacing between the label row and subtitle row.
     *
     * @since 5.83
     * @since org.kde.kirigami 2.15
     * @property real textSpacing
     */
    property alias textSpacing: labelColumn.spacing

    /**
     * This property holds whether to make the icon and labels have a disabled look. Defaults to
     * false. Can be used to tweak whether the content elements are visually
     * active while preserving an active appearance for any leading or trailing
     * items.
     *
     * @since 5.83
     * @since org.kde.kirigami 2.15
     */
    property bool fadeContent: false

    /**
     * This property holds the label item, for accessing the usual Text properties.
     *
     * @property QtQuick.Controls.Label labelItem
     * @since 5.84
     * @since org.kde.kirigami 2.16
     */
    property alias labelItem: labelItem

    /**
     * This property holds the subtitle item, for accessing the usual Text properties.
     *
     * @property QtQuick.Controls.Label subtitleItem
     * @since 5.84
     * @since org.kde.kirigami 2.16
     */
    property alias subtitleItem: subtitleItem

    default property alias _basicDefault: layout.data

    icon: action ? action.icon.name || action.icon.source : undefined

    contentItem: Item {
        id: contItem
        implicitWidth: (listItem.leading || {implicitWidth: 0}).implicitWidth + layout.implicitWidth + (listItem.trailing || {implicitWidth: 0}).implicitWidth
        Binding on implicitHeight {
            value: Math.max(iconItem.size, (!subtitleItem.visible && listItem.reserveSpaceForSubtitle ? (labelItem.implicitHeight + labelColumn.spacing + subtitleItem.implicitHeight): labelColumn.implicitHeight) )
            delayed: true
        }

        RowLayout {
            id: layout
            spacing: LayoutMirroring.enabled ? listItem.rightPadding : listItem.leftPadding
            anchors.left: contItem.left
            anchors.leftMargin: listItem.leading ? listItem.leadingPadding : 0
            anchors.right: contItem.right
            anchors.rightMargin: listItem.trailing ? listItem.trailingPadding : 0
            anchors.verticalCenter: parent.verticalCenter

            Icon {
                id: iconItem
                source: {
                    if (!listItem.icon) {
                        return undefined
                    }
                    if (listItem.icon.hasOwnProperty) {
                        if (listItem.icon.hasOwnProperty("name") && listItem.icon.name !== "")
                            return listItem.icon.name;
                        if (listItem.icon.hasOwnProperty("source"))
                            return listItem.icon.source;
                    }
                    return listItem.icon;
                }
                property int size: subtitleItem.visible || reserveSpaceForSubtitle ? Units.iconSizes.medium : Units.iconSizes.smallMedium
                Layout.minimumHeight: size
                Layout.maximumHeight: size
                Layout.minimumWidth: size
                Layout.maximumWidth: size
                selected: (listItem.highlighted || listItem.checked || (listItem.pressed && listItem.supportsMouseEvents))
                opacity: listItem.fadeContent ? 0.4 : 1.0
                visible: source !== undefined
            }
            ColumnLayout {
                id: labelColumn
                spacing: 0
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignVCenter
                QQC2.Label {
                    id: labelItem
                    text: listItem.text
                    Layout.fillWidth: true
                    Layout.alignment: subtitleItem.visible ? Qt.AlignLeft | Qt.AlignBottom : Qt.AlignLeft | Qt.AlignVCenter
                    color: (listItem.highlighted || listItem.checked || (listItem.pressed && listItem.supportsMouseEvents)) ? listItem.activeTextColor : listItem.textColor
                    elide: Text.ElideRight
                    font.weight: listItem.bold ? Font.Bold : Font.Normal
                    opacity: listItem.fadeContent ? 0.4 : 1.0
                }
                QQC2.Label {
                    id: subtitleItem
                    Layout.fillWidth: true
                    Layout.alignment: subtitleItem.visible ? Qt.AlignLeft | Qt.AlignTop : Qt.AlignLeft | Qt.AlignVCenter
                    color: (listItem.highlighted || listItem.checked || (listItem.pressed && listItem.supportsMouseEvents)) ? listItem.activeTextColor : listItem.textColor
                    elide: Text.ElideRight
                    font: Theme.smallFont
                    opacity: listItem.bold
                        ? (listItem.fadeContent ? 0.3 : 0.9)
                        : (listItem.fadeContent ? 0.1 : 0.7)
                    visible: text.length > 0
                }
            }
        }
    }

    Keys.onEnterPressed: action ? action.trigger() : clicked()
    Keys.onReturnPressed: action ? action.trigger() : clicked()
}
