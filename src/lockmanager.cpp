/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "lockmanager.h"
#include "screensaverdbusinterface.h"

#include <KLocalizedString>

#include <QDBusConnection>

using namespace Qt::StringLiterals;

LockManager::LockManager(QObject *parent)
    : QObject(parent)
{
    m_iface = new OrgFreedesktopScreenSaverInterface(u"org.freedesktop.ScreenSaver"_s, u"/org/freedesktop/ScreenSaver"_s, QDBusConnection::sessionBus(), this);
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
    m_cookie = m_iface->Inhibit(u"org.kde.haruna"_s, i18nc("@info reason for blocking sleep and screen slocking", "Playing media file"));
}

#include "moc_lockmanager.cpp"
