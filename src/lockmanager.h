/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef LOCKMANAGER_H
#define LOCKMANAGER_H

#include <QObject>

#include "config-haruna.h"

class OrgFreedesktopScreenSaverInterface;

class LockManager : public QObject
{
    Q_OBJECT

public:
    explicit LockManager(QObject *parent = nullptr);
    ~LockManager() = default;

public Q_SLOTS:
    void setInhibitionOn();
    void setInhibitionOff();

private:
#if HAVE_DBUS
    OrgFreedesktopScreenSaverInterface *m_iface{nullptr};
    int m_cookie{-1};
#endif
};

#endif // LOCKMANAGER_H
