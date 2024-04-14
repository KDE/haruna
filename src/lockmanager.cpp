/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "lockmanager.h"
#include "screensaverdbusinterface.h"

#include <KLocalizedString>

#include <QDBusConnection>

LockManager::LockManager(QObject *parent)
    : QObject(parent)
{
    m_iface = new OrgFreedesktopScreenSaverInterface(QStringLiteral("org.freedesktop.ScreenSaver"),
                                                     QStringLiteral("/org/freedesktop/ScreenSaver"),
                                                     QDBusConnection::sessionBus(),
                                                     this);
    setInhibitionOn();
}

void LockManager::setInhibitionOff()
{
    m_iface->UnInhibit(m_cookie);
    m_cookie = -1;
}

void LockManager::setInhibitionOn()
{
    if (m_cookie != -1) {
        setInhibitionOff();
    }
    m_cookie = m_iface->Inhibit(QStringLiteral("org.kde.haruna"), i18nc("@info reason for blocking sleep and screen slocking", "Playing media file"));
}

#include "moc_lockmanager.cpp"
