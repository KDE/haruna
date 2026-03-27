/*
 * SPDX-FileCopyrightText: 2025 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "systemutils.h"

#include <QFontDatabase>
#include <QGuiApplication>

#include <KIO/ApplicationLauncherJob>
#include <KService>
#include <KWindowSystem>

using namespace Qt::StringLiterals;

SystemUtils::SystemUtils(QObject *parent)
    : QObject{parent}
{

}

QStringList SystemUtils::getFonts()
{
    return QFontDatabase::families();
}

QString SystemUtils::platformName()
{
    return QGuiApplication::platformName();
}

bool SystemUtils::isPlatformWayland()
{
    return KWindowSystem::isPlatformWayland();
}

bool SystemUtils::isHanaInstalled()
{
    auto service = KService::serviceByDesktopName(u"org.kde.hana"_s);
    if (!service) {
        return false;
    }

    return true;
}

bool SystemUtils::openHana(QUrl url)
{
    auto service = KService::serviceByDesktopName(u"org.kde.hana"_s);
    if (!service) {
        return false;
    }

    auto *job = new KIO::ApplicationLauncherJob(service);
    job->setUrls(QList<QUrl>() << url);
    job->start();

    return true;
}

bool SystemUtils::isMediaInfoInstalled()
{
    auto service1 = KService::serviceByDesktopName(u"net.mediaarea.MediaInfo"_s);
    if (service1) {
        return true;
    }
    auto service2 = KService::serviceByDesktopName(u"mediainfo-gui"_s);
    if (service2) {
        return true;
    }

    return false;
}

bool SystemUtils::openMediaInfo(QUrl url)
{
    auto service1 = KService::serviceByDesktopName(u"net.mediaarea.MediaInfo"_s);
    if (service1) {
        auto *job = new KIO::ApplicationLauncherJob(service1);
        job->setUrls(QList<QUrl>() << url);
        job->start();
        return true;
    }

    auto service2 = KService::serviceByDesktopName(u"mediainfo-gui"_s);
    if (service2) {
        auto *job = new KIO::ApplicationLauncherJob(service2);
        job->setUrls(QList<QUrl>() << url);
        job->start();
        return true;
    }

    return false;
}
