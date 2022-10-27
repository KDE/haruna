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

    title: i18nc("@title:menu", "&Video")

    Labs.MenuItem {
        icon.name: appActions.toggleDeinterlacingAction.icon.name
        text: appActions.toggleDeinterlacingAction.text
        onTriggered: appActions.toggleDeinterlacingAction.trigger()
    }

    Labs.MenuItem {
        icon.name: appActions.zoomInAction.icon.name
        text: appActions.zoomInAction.text
        onTriggered: appActions.zoomInAction.trigger()
    }

    Labs.MenuItem {
        icon.name: appActions.zoomOutAction.icon.name
        text: appActions.zoomOutAction.text
        onTriggered: appActions.zoomOutAction.trigger()
    }

    Labs.MenuItem {
        icon.name: appActions.zoomResetAction.icon.name
        text: appActions.zoomResetAction.text
        onTriggered: appActions.zoomResetAction.trigger()
    }

    Labs.MenuItem {
        icon.name: appActions.screenshotAction.icon.name
        text: appActions.screenshotAction.text
        onTriggered: appActions.screenshotAction.trigger()
    }

    Labs.MenuItem {
        icon.name: appActions.videoPanXLeftAction.icon.name
        text: appActions.videoPanXLeftAction.text
        onTriggered: appActions.videoPanXLeftAction.trigger()
    }

    Labs.MenuItem {
        icon.name: appActions.videoPanXRightAction.icon.name
        text: appActions.videoPanXRightAction.text
        onTriggered: appActions.videoPanXRightAction.trigger()
    }

    Labs.MenuItem {
        icon.name: appActions.videoPanYUpAction.icon.name
        text: appActions.videoPanYUpAction.text
        onTriggered: appActions.videoPanYUpAction.trigger()
    }

    Labs.MenuItem {
        icon.name: appActions.videoPanYDownAction.icon.name
        text: appActions.videoPanYDownAction.text
        onTriggered: appActions.videoPanYDownAction.trigger()
    }

    Labs.Menu {
        title: i18nc("@title:menu", "Adjustments")

        Labs.MenuItem {
            icon.name: appActions.contrastUpAction.icon.name
            text: appActions.contrastUpAction.text
            onTriggered: appActions.contrastUpAction.trigger()
        }

        Labs.MenuItem {
            icon.name: appActions.contrastDownAction.icon.name
            text: appActions.contrastDownAction.text
            onTriggered: appActions.contrastDownAction.trigger()
        }

        Labs.MenuItem {
            icon.name: appActions.contrastResetAction.icon.name
            text: appActions.contrastResetAction.text
            onTriggered: appActions.contrastResetAction.trigger()
        }

        Labs.MenuItem {
            icon.name: appActions.brightnessUpAction.icon.name
            text: appActions.brightnessUpAction.text
            onTriggered: appActions.brightnessUpAction.trigger()
        }

        Labs.MenuItem {
            icon.name: appActions.brightnessDownAction.icon.name
            text: appActions.brightnessDownAction.text
            onTriggered: appActions.brightnessDownAction.trigger()
        }

        Labs.MenuItem {
            icon.name: appActions.brightnessResetAction.icon.name
            text: appActions.brightnessResetAction.text
            onTriggered: appActions.brightnessResetAction.trigger()
        }

        Labs.MenuItem {
            icon.name: appActions.gammaUpAction.icon.name
            text: appActions.gammaUpAction.text
            onTriggered: appActions.gammaUpAction.trigger()
        }

        Labs.MenuItem {
            icon.name: appActions.gammaDownAction.icon.name
            text: appActions.gammaDownAction.text
            onTriggered: appActions.gammaDownAction.trigger()
        }

        Labs.MenuItem {
            icon.name: appActions.gammaResetAction.icon.name
            text: appActions.gammaResetAction.text
            onTriggered: appActions.gammaResetAction.trigger()
        }

        Labs.MenuItem {
            icon.name: appActions.saturationUpAction.icon.name
            text: appActions.saturationUpAction.text
            onTriggered: appActions.saturationUpAction.trigger()
        }
        Labs.MenuItem {
            icon.name: appActions.saturationDownAction.icon.name
            text: appActions.saturationDownAction.text
            onTriggered: appActions.saturationDownAction.trigger()
        }
        Labs.MenuItem {
            icon.name: appActions.saturationResetAction.icon.name
            text: appActions.saturationResetAction.text
            onTriggered: appActions.saturationResetAction.trigger()
        }
    }
}
