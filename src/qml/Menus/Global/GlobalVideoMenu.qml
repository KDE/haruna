/*
 * SPDX-FileCopyrightText: 2022 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import Qt.labs.platform as Labs

import org.kde.haruna

Labs.Menu {
    id: root

    title: i18nc("@title:menu", "&Video")

    Labs.MenuItem {
        icon.name: HarunaApp.actions.toggleDeinterlacingAction.icon.name
        text: HarunaApp.actions.toggleDeinterlacingAction.text
        onTriggered: HarunaApp.actions.toggleDeinterlacingAction.trigger()
    }

    Labs.MenuItem {
        icon.name: HarunaApp.actions.zoomInAction.icon.name
        text: HarunaApp.actions.zoomInAction.text
        onTriggered: HarunaApp.actions.zoomInAction.trigger()
    }

    Labs.MenuItem {
        icon.name: HarunaApp.actions.zoomOutAction.icon.name
        text: HarunaApp.actions.zoomOutAction.text
        onTriggered: HarunaApp.actions.zoomOutAction.trigger()
    }

    Labs.MenuItem {
        icon.name: HarunaApp.actions.zoomResetAction.icon.name
        text: HarunaApp.actions.zoomResetAction.text
        onTriggered: HarunaApp.actions.zoomResetAction.trigger()
    }

    Labs.MenuItem {
        icon.name: HarunaApp.actions.screenshotAction.icon.name
        text: HarunaApp.actions.screenshotAction.text
        onTriggered: HarunaApp.actions.screenshotAction.trigger()
    }

    Labs.MenuItem {
        icon.name: HarunaApp.actions.rotateClockwiseAction.icon.name
        text: HarunaApp.actions.rotateClockwiseAction.text
        onTriggered: HarunaApp.actions.rotateClockwiseAction.trigger()
    }

    Labs.MenuItem {
        icon.name: HarunaApp.actions.rotateCounterClockwiseAction.icon.name
        text: HarunaApp.actions.rotateCounterClockwiseAction.text
        onTriggered: HarunaApp.actions.rotateCounterClockwiseAction.trigger()
    }

    Labs.MenuItem {
        icon.name: HarunaApp.actions.videoPanXLeftAction.icon.name
        text: HarunaApp.actions.videoPanXLeftAction.text
        onTriggered: HarunaApp.actions.videoPanXLeftAction.trigger()
    }

    Labs.MenuItem {
        icon.name: HarunaApp.actions.videoPanXRightAction.icon.name
        text: HarunaApp.actions.videoPanXRightAction.text
        onTriggered: HarunaApp.actions.videoPanXRightAction.trigger()
    }

    Labs.MenuItem {
        icon.name: HarunaApp.actions.videoPanYUpAction.icon.name
        text: HarunaApp.actions.videoPanYUpAction.text
        onTriggered: HarunaApp.actions.videoPanYUpAction.trigger()
    }

    Labs.MenuItem {
        icon.name: HarunaApp.actions.videoPanYDownAction.icon.name
        text: HarunaApp.actions.videoPanYDownAction.text
        onTriggered: HarunaApp.actions.videoPanYDownAction.trigger()
    }

    Labs.Menu {
        title: i18nc("@title:menu", "Adjustments")

        Labs.MenuItem {
            icon.name: HarunaApp.actions.contrastUpAction.icon.name
            text: HarunaApp.actions.contrastUpAction.text
            onTriggered: HarunaApp.actions.contrastUpAction.trigger()
        }

        Labs.MenuItem {
            icon.name: HarunaApp.actions.contrastDownAction.icon.name
            text: HarunaApp.actions.contrastDownAction.text
            onTriggered: HarunaApp.actions.contrastDownAction.trigger()
        }

        Labs.MenuItem {
            icon.name: HarunaApp.actions.contrastResetAction.icon.name
            text: HarunaApp.actions.contrastResetAction.text
            onTriggered: HarunaApp.actions.contrastResetAction.trigger()
        }

        Labs.MenuItem {
            icon.name: HarunaApp.actions.brightnessUpAction.icon.name
            text: HarunaApp.actions.brightnessUpAction.text
            onTriggered: HarunaApp.actions.brightnessUpAction.trigger()
        }

        Labs.MenuItem {
            icon.name: HarunaApp.actions.brightnessDownAction.icon.name
            text: HarunaApp.actions.brightnessDownAction.text
            onTriggered: HarunaApp.actions.brightnessDownAction.trigger()
        }

        Labs.MenuItem {
            icon.name: HarunaApp.actions.brightnessResetAction.icon.name
            text: HarunaApp.actions.brightnessResetAction.text
            onTriggered: HarunaApp.actions.brightnessResetAction.trigger()
        }

        Labs.MenuItem {
            icon.name: HarunaApp.actions.gammaUpAction.icon.name
            text: HarunaApp.actions.gammaUpAction.text
            onTriggered: HarunaApp.actions.gammaUpAction.trigger()
        }

        Labs.MenuItem {
            icon.name: HarunaApp.actions.gammaDownAction.icon.name
            text: HarunaApp.actions.gammaDownAction.text
            onTriggered: HarunaApp.actions.gammaDownAction.trigger()
        }

        Labs.MenuItem {
            icon.name: HarunaApp.actions.gammaResetAction.icon.name
            text: HarunaApp.actions.gammaResetAction.text
            onTriggered: HarunaApp.actions.gammaResetAction.trigger()
        }

        Labs.MenuItem {
            icon.name: HarunaApp.actions.saturationUpAction.icon.name
            text: HarunaApp.actions.saturationUpAction.text
            onTriggered: HarunaApp.actions.saturationUpAction.trigger()
        }
        Labs.MenuItem {
            icon.name: HarunaApp.actions.saturationDownAction.icon.name
            text: HarunaApp.actions.saturationDownAction.text
            onTriggered: HarunaApp.actions.saturationDownAction.trigger()
        }
        Labs.MenuItem {
            icon.name: HarunaApp.actions.saturationResetAction.icon.name
            text: HarunaApp.actions.saturationResetAction.text
            onTriggered: HarunaApp.actions.saturationResetAction.trigger()
        }
    }
}
