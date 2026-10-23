/*
 * SPDX-FileCopyrightText: 2026 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#define TRANSLATION_DOMAIN "haruna"

#include "osdutils.h"

#include <QQmlEngine>

#include <KLocalizedString>

#include "osdsettings.h"

using namespace Qt::StringLiterals;

using Function = bool (*)();

// clang-format off
QHash<OsdUtils::Context, Function> functions()
{
    static const QHash<OsdUtils::Context, Function> _functions = {
        { OsdUtils::Context::MpvABLoop,           &OsdSettings::mpvABLoop },
        { OsdUtils::Context::MpvChapter,          &OsdSettings::mpvChapter },
        { OsdUtils::Context::MpvDeinterlace,      &OsdSettings::mpvDeinterlace },
        { OsdUtils::Context::MpvImageAdjustments, &OsdSettings::mpvImageAdjustments },
        { OsdUtils::Context::MpvLoadFile,         &OsdSettings::mpvLoadFile },
        { OsdUtils::Context::MpvPlaybackSpeed,    &OsdSettings::mpvPlaybackSpeed },
        { OsdUtils::Context::MpvScreenshot,       &OsdSettings::mpvScreenshot },
        { OsdUtils::Context::MpvSubtitleDelay,    &OsdSettings::mpvSubtitleDelay },
        { OsdUtils::Context::MpvSubtitleScale,    &OsdSettings::mpvSubtitleScale },
        { OsdUtils::Context::MpvSubtitleToggle,   &OsdSettings::mpvSubtitleToggle },
        { OsdUtils::Context::MpvTrack,            &OsdSettings::mpvTrack },
        { OsdUtils::Context::MpvVolume,           &OsdSettings::mpvVolume },
    };

    return _functions;
}

QHash<QString, QString> labels()
{
    static const QHash<QString, QString> _labels = {
        { u"mpvABLoop"_s,           i18nc("@option:check", "AB loop") },
        { u"mpvChapter"_s,          i18nc("@option:check", "Skip chapter") },
        { u"mpvDeinterlace"_s,      i18nc("@option:check", "Toggle deinterlace") },
        { u"mpvImageAdjustments"_s, i18nc("@option:check", "Image adjustments (contrast, brightness, gamma and saturation)") },
        { u"mpvLoadFile"_s,         i18nc("@option:check", "Playing new file") },
        { u"mpvPlaybackSpeed"_s,    i18nc("@option:check", "Playback speed change") },
        { u"mpvScreenshot"_s,       i18nc("@option:check", "Taking screenshot") },
        { u"mpvSubtitleDelay"_s,    i18nc("@option:check", "Subtitle timing change") },
        { u"mpvSubtitleScale"_s,    i18nc("@option:check", "Subtitle scale change") },
        { u"mpvSubtitleToggle"_s,   i18nc("@option:check", "Toggle subtitle") },
        { u"mpvTrack"_s,            i18nc("@option:check", "Track change") },
        { u"mpvVolume"_s,           i18nc("@option:check", "Volume change") },
    };

    return _labels;
}
// clang-format on

OsdUtils &OsdUtils::instance()
{
    static OsdUtils i;
    return i;
}

OsdUtils *OsdUtils::create(QQmlEngine *, QJSEngine *)
{
    QQmlEngine::setObjectOwnership(&instance(), QQmlEngine::CppOwnership);
    return &instance();
}

OsdUtils::OsdUtils(QObject *parent)
    : QObject{parent}
{
}

void OsdUtils::message(const QString &message, Context context)
{
    if (context == OsdUtils::Context::None) {
        Q_EMIT messageReceived(message, context);
        return;
    }

    const auto f = functions();
    if (auto it = f.constFind(context); it != f.cend()) {
        if (it.value()()) {
            Q_EMIT messageReceived(message, context);
        }
    }
}

QString OsdUtils::label(const QString &key)
{
    return labels().value(key);
}

QStringList OsdUtils::options()
{
    static QStringList keys;
    if (keys.isEmpty()) {
        keys = labels().keys();
        keys.sort();
    }
    return keys;
}

#include "moc_osdutils.cpp"
