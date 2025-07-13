/*
 * SPDX-FileCopyrightText: 2021 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick.Controls

import org.kde.haruna

Menu {
    id: root

    title: i18nc("@title:menu", "&Video")

    MenuItem { action: HarunaApp.actions.toggleDeinterlacingAction }
    MenuItem { action: HarunaApp.actions.zoomInAction }
    MenuItem { action: HarunaApp.actions.zoomOutAction }
    MenuItem { action: HarunaApp.actions.zoomResetAction }
    MenuItem { action: HarunaApp.actions.screenshotAction }
    MenuItem { action: HarunaApp.actions.rotateClockwiseAction }
    MenuItem { action: HarunaApp.actions.rotateCounterClockwiseAction }
    MenuItem { action: HarunaApp.actions.videoPanXLeftAction }
    MenuItem { action: HarunaApp.actions.videoPanXRightAction }
    MenuItem { action: HarunaApp.actions.videoPanYUpAction }
    MenuItem { action: HarunaApp.actions.videoPanYDownAction }

    Menu {
        title: i18nc("@title:menu", "Adjustments")

        MenuItem { action: HarunaApp.actions.contrastUpAction }
        MenuItem { action: HarunaApp.actions.contrastDownAction }
        MenuItem { action: HarunaApp.actions.contrastResetAction }
        MenuItem { action: HarunaApp.actions.brightnessUpAction }
        MenuItem { action: HarunaApp.actions.brightnessDownAction }
        MenuItem { action: HarunaApp.actions.brightnessResetAction }
        MenuItem { action: HarunaApp.actions.gammaUpAction }
        MenuItem { action: HarunaApp.actions.gammaDownAction }
        MenuItem { action: HarunaApp.actions.gammaResetAction }
        MenuItem { action: HarunaApp.actions.saturationUpAction }
        MenuItem { action: HarunaApp.actions.saturationDownAction }
        MenuItem { action: HarunaApp.actions.saturationResetAction }
    }
}
