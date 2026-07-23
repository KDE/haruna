/*
 * SPDX-FileCopyrightText: 2021 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "rootadaptor.h"

#include <QDBusConnection>
#include <QDBusMessage>

#include "mpriscontroller.h"

using namespace Qt::StringLiterals;

RootAdaptor::RootAdaptor(MprisController &parent)
    : QDBusAbstractAdaptor(&parent)
    , m_mprisController(parent)
{
}

void RootAdaptor::notify(const QString &property, const QVariant &value)
{
    const auto path = u"/org/mpris/MediaPlayer2"_s;
    const auto propertiesInterface = u"org.freedesktop.DBus.Properties"_s;
    const auto rootInterface = u"org.mpris.MediaPlayer2"_s;
    const auto properties = QVariantMap{{property, value}};

    QDBusMessage msg = QDBusMessage::createSignal(path, propertiesInterface, u"PropertiesChanged"_s);
    msg << rootInterface << properties << QStringList{};

    QDBusConnection::sessionBus().send(msg);
}

bool RootAdaptor::canQuit() const
{
    return true;
}

bool RootAdaptor::fullscreen() const
{
    return m_mprisController.fullscreen();
}

void RootAdaptor::setFullscreen(bool newFullscreen)
{
    m_mprisController.requestFullscreenChange(newFullscreen);
}

bool RootAdaptor::canSetFullscreen() const
{
    return m_mprisController.canSetFullscreen();
}

bool RootAdaptor::canRaise() const
{
    return m_mprisController.canRaise();
}

bool RootAdaptor::hasTrackList() const
{
    return m_mprisController.hasTrackList();
}

QString RootAdaptor::identity() const
{
    return m_mprisController.identity();
}

QString RootAdaptor::desktopEntry() const
{
    return m_mprisController.desktopEntry();
}

QStringList RootAdaptor::supportedUriSchemes() const
{
    return m_mprisController.supportedUriSchemes();
}

QStringList RootAdaptor::supportedMimeTypes() const
{
    return m_mprisController.supportedMimeTypes();
}

void RootAdaptor::Raise()
{
    m_mprisController.raise();
}

void RootAdaptor::Quit()
{
    m_mprisController.quit();
}

#include "moc_rootadaptor.cpp"
