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

    MenuItem { action: appActions.toggleDeinterlacingAction }
    MenuItem { action: appActions.zoomInAction }
    MenuItem { action: appActions.zoomOutAction }
    MenuItem { action: appActions.zoomResetAction }
    MenuItem { action: appActions.screenshotAction }
    MenuItem { action: appActions.videoPanXLeftAction }
    MenuItem { action: appActions.videoPanXRightAction }
    MenuItem { action: appActions.videoPanYUpAction }
    MenuItem { action: appActions.videoPanYDownAction }

    Menu {
        title: i18n("Adjustments")

        MenuItem { action: appActions.contrastUpAction }
        MenuItem { action: appActions.contrastDownAction }
        MenuItem { action: appActions.contrastResetAction }
        MenuItem { action: appActions.brightnessUpAction }
        MenuItem { action: appActions.brightnessDownAction }
        MenuItem { action: appActions.brightnessResetAction }
        MenuItem { action: appActions.gammaUpAction }
        MenuItem { action: appActions.gammaDownAction }
        MenuItem { action: appActions.gammaResetAction }
        MenuItem { action: appActions.saturationUpAction }
        MenuItem { action: appActions.saturationDownAction }
        MenuItem { action: appActions.saturationResetAction }
    }
}
