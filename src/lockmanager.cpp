/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "lockmanager.h"

#include <QDBusConnection>

#include <KLocalizedString>

#include "screensaverdbusinterface.h"

using namespace Qt::StringLiterals;

LockManager::LockManager(QObject *parent)
    : QObject(parent)
{
#if defined(Q_OS_UNIX)
    m_iface = new OrgFreedesktopScreenSaverInterface(u"org.freedesktop.ScreenSaver"_s, u"/org/freedesktop/ScreenSaver"_s, QDBusConnection::sessionBus(), this);
#endif
}

void LockManager::setInhibitionOff()
{
#if defined(Q_OS_UNIX)
    m_iface->UnInhibit(m_cookie);
    m_cookie = -1;
#endif

#if defined(Q_OS_WIN32)
    SetThreadExecutionState(ES_CONTINUOUS);
#endif
}

void LockManager::setInhibitionOn()
{
#if defined(Q_OS_UNIX)
    if (m_cookie != -1) {
        setInhibitionOff();
    }
    m_cookie = m_iface->Inhibit(u"org.kde.haruna"_s, i18nc("@info reason for blocking sleep and screen slocking", "Playing media file"));
#endif

#if defined(Q_OS_WIN32)
    SetThreadExecutionState(ES_CONTINUOUS | ES_DISPLAY_REQUIRED);
#endif
}

#include "moc_lockmanager.cpp"
