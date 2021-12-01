/*
 * SPDX-FileCopyrightText: 2021 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.12
import QtQuick.Controls 2.12

Menu {
    id: root

    title: i18n("&Video")

    MenuItem { action: actions.toggleDeinterlacingAction }
    MenuItem { action: actions.zoomInAction }
    MenuItem { action: actions.zoomOutAction }
    MenuItem { action: actions.zoomResetAction }
    MenuItem { action: actions.screenshotAction }
    MenuItem { action: actions.videoPanXLeftAction }
    MenuItem { action: actions.videoPanXRightAction }
    MenuItem { action: actions.videoPanYUpAction }
    MenuItem { action: actions.videoPanYDownAction }

    Menu {
        title: i18n("Adjustments")

        MenuItem { action: actions.contrastUpAction }
        MenuItem { action: actions.contrastDownAction }
        MenuItem { action: actions.contrastResetAction }
        MenuItem { action: actions.brightnessUpAction }
        MenuItem { action: actions.brightnessDownAction }
        MenuItem { action: actions.brightnessResetAction }
        MenuItem { action: actions.gammaUpAction }
        MenuItem { action: actions.gammaDownAction }
        MenuItem { action: actions.gammaResetAction }
        MenuItem { action: actions.saturationUpAction }
        MenuItem { action: actions.saturationDownAction }
        MenuItem { action: actions.saturationResetAction }
    }
}
