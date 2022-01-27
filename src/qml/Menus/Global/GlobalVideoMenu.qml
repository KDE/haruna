/*
 * SPDX-FileCopyrightText: 2022 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.12
import QtQuick.Controls 2.12
import Qt.labs.platform 1.1 as Labs

Labs.Menu {
    id: root

    title: i18n("&Video")

    Labs.MenuItem {
        icon.name: actions.toggleDeinterlacingAction.icon.name
        text: actions.toggleDeinterlacingAction.text
        onTriggered: actions.toggleDeinterlacingAction.trigger()
    }

    Labs.MenuItem {
        icon.name: actions.zoomInAction.icon.name
        text: actions.zoomInAction.text
        onTriggered: actions.zoomInAction.trigger()
    }

    Labs.MenuItem {
        icon.name: actions.zoomOutAction.icon.name
        text: actions.zoomOutAction.text
        onTriggered: actions.zoomOutAction.trigger()
    }

    Labs.MenuItem {
        icon.name: actions.zoomResetAction.icon.name
        text: actions.zoomResetAction.text
        onTriggered: actions.zoomResetAction.trigger()
    }

    Labs.MenuItem {
        icon.name: actions.screenshotAction.icon.name
        text: actions.screenshotAction.text
        onTriggered: actions.screenshotAction.trigger()
    }

    Labs.MenuItem {
        icon.name: actions.videoPanXLeftAction.icon.name
        text: actions.videoPanXLeftAction.text
        onTriggered: actions.videoPanXLeftAction.trigger()
    }

    Labs.MenuItem {
        icon.name: actions.videoPanXRightAction.icon.name
        text: actions.videoPanXRightAction.text
        onTriggered: actions.videoPanXRightAction.trigger()
    }

    Labs.MenuItem {
        icon.name: actions.videoPanYUpAction.icon.name
        text: actions.videoPanYUpAction.text
        onTriggered: actions.videoPanYUpAction.trigger()
    }

    Labs.MenuItem {
        icon.name: actions.videoPanYDownAction.icon.name
        text: actions.videoPanYDownAction.text
        onTriggered: actions.videoPanYDownAction.trigger()
    }

    Labs.Menu {
        title: i18n("Adjustments")

        Labs.MenuItem {
            icon.name: actions.contrastUpAction.icon.name
            text: actions.contrastUpAction.text
            onTriggered: actions.contrastUpAction.trigger()
        }

        Labs.MenuItem {
            icon.name: actions.contrastDownAction.icon.name
            text: actions.contrastDownAction.text
            onTriggered: actions.contrastDownAction.trigger()
        }

        Labs.MenuItem {
            icon.name: actions.contrastResetAction.icon.name
            text: actions.contrastResetAction.text
            onTriggered: actions.contrastResetAction.trigger()
        }

        Labs.MenuItem {
            icon.name: actions.brightnessUpAction.icon.name
            text: actions.brightnessUpAction.text
            onTriggered: actions.brightnessUpAction.trigger()
        }

        Labs.MenuItem {
            icon.name: actions.brightnessDownAction.icon.name
            text: actions.brightnessDownAction.text
            onTriggered: actions.brightnessDownAction.trigger()
        }

        Labs.MenuItem {
            icon.name: actions.brightnessResetAction.icon.name
            text: actions.brightnessResetAction.text
            onTriggered: actions.brightnessResetAction.trigger()
        }

        Labs.MenuItem {
            icon.name: actions.gammaUpAction.icon.name
            text: actions.gammaUpAction.text
            onTriggered: actions.gammaUpAction.trigger()
        }

        Labs.MenuItem {
            icon.name: actions.gammaDownAction.icon.name
            text: actions.gammaDownAction.text
            onTriggered: actions.gammaDownAction.trigger()
        }

        Labs.MenuItem {
            icon.name: actions.gammaResetAction.icon.name
            text: actions.gammaResetAction.text
            onTriggered: actions.gammaResetAction.trigger()
        }

        Labs.MenuItem {
            icon.name: actions.saturationUpAction.icon.name
            text: actions.saturationUpAction.text
            onTriggered: actions.saturationUpAction.trigger()
        }
        Labs.MenuItem {
            icon.name: actions.saturationDownAction.icon.name
            text: actions.saturationDownAction.text
            onTriggered: actions.saturationDownAction.trigger()
        }
        Labs.MenuItem {
            icon.name: actions.saturationResetAction.icon.name
            text: actions.saturationResetAction.text
            onTriggered: actions.saturationResetAction.trigger()
        }
    }
}
