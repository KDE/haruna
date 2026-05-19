/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "lockmanager.h"

#include <KLocalizedString>

#if HAVE_DBUS
#include <QDBusConnection>

#include "screensaverdbusinterface.h"
#endif

#ifdef Q_OS_WIN32
#include <Windows.h>
#endif

using namespace Qt::StringLiterals;

LockManager::LockManager(QObject *parent)
    : QObject(parent)
{
#if HAVE_DBUS
    m_iface = new OrgFreedesktopScreenSaverInterface(u"org.freedesktop.ScreenSaver"_s, u"/org/freedesktop/ScreenSaver"_s, QDBusConnection::sessionBus(), this);
#endif
}

void LockManager::setInhibitionOff()
{
#if HAVE_DBUS
    m_iface->UnInhibit(m_cookie);
    m_cookie = -1;
#endif

#ifdef Q_OS_WIN32
    SetThreadExecutionState(ES_CONTINUOUS);
#endif
}

void LockManager::setInhibitionOn()
{
#if HAVE_DBUS
    if (m_cookie != -1) {
        setInhibitionOff();
    }
    m_cookie = m_iface->Inhibit(u"org.kde.haruna"_s, i18nc("@info reason for blocking sleep and screen slocking", "Playing media file"));
#endif

#ifdef Q_OS_WIN32
    SetThreadExecutionState(ES_CONTINUOUS | ES_DISPLAY_REQUIRED);
#endif
}

#include "moc_lockmanager.cpp"
