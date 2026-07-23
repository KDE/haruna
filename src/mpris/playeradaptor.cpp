/*
 * SPDX-FileCopyrightText: 2021 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "playeradaptor.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusObjectPath>

#include "mpriscontroller.h"

using namespace Qt::StringLiterals;

PlayerAdaptor::PlayerAdaptor(MprisController &parent)
    : QDBusAbstractAdaptor(&parent)
    , m_mprisController(parent)
{
}

void PlayerAdaptor::notify(const QString &property, const QVariant &value)
{
    const auto path = u"/org/mpris/MediaPlayer2"_s;
    const auto propertiesInterface = u"org.freedesktop.DBus.Properties"_s;
    const auto playerInterface = u"org.mpris.MediaPlayer2.Player"_s;
    const auto properties = QVariantMap{{property, value}};

    QDBusMessage msg = QDBusMessage::createSignal(path, propertiesInterface, u"PropertiesChanged"_s);
    msg << playerInterface << properties << QStringList{};

    QDBusConnection::sessionBus().send(msg);
}

void PlayerAdaptor::seeked(qlonglong position)
{
    Q_EMIT Seeked(position);
}

QString PlayerAdaptor::playbackStatus()
{
    return m_mprisController.playbackStatus();
}

QString PlayerAdaptor::loopStatus()
{
    return m_mprisController.loopStatus();
}

void PlayerAdaptor::setLoopStatus(const QString &newLoopStatus)
{
    m_mprisController.requestLoopStatusChange(newLoopStatus);
}

double PlayerAdaptor::rate() const
{
    return m_mprisController.rate();
}

void PlayerAdaptor::setRate(double newRate)
{
    m_mprisController.requestRateChange(newRate);
}

bool PlayerAdaptor::shuffle()
{
    return m_mprisController.shuffle();
}

void PlayerAdaptor::setShuffle(bool newShuffle)
{
    m_mprisController.requestShuffleChange(newShuffle);
}

QVariantMap PlayerAdaptor::metadata()
{
    return m_mprisController.metadata();
}

double PlayerAdaptor::volume()
{
    return m_mprisController.volume();
}

void PlayerAdaptor::setVolume(double newVolume)
{
    m_mprisController.requestVolumeChange(newVolume);
}

qlonglong PlayerAdaptor::position()
{
    return m_mprisController.position();
}

double PlayerAdaptor::minimumRate() const
{
    return m_mprisController.minimumRate();
}

double PlayerAdaptor::maximumRate() const
{
    return m_mprisController.maximumRate();
}

bool PlayerAdaptor::canGoNext()
{
    return m_mprisController.canGoNext();
}

bool PlayerAdaptor::canGoPrevious()
{
    return m_mprisController.canGoPrevious();
}

bool PlayerAdaptor::canPlay()
{
    return m_mprisController.canPlay();
}

bool PlayerAdaptor::canPause()
{
    return m_mprisController.canPause();
}

bool PlayerAdaptor::canSeek()
{
    return m_mprisController.canSeek();
}

bool PlayerAdaptor::canControl()
{
    return m_mprisController.canControl();
}

void PlayerAdaptor::Next()
{
    m_mprisController.next();
}

void PlayerAdaptor::Previous()
{
    m_mprisController.previous();
}

void PlayerAdaptor::Pause()
{
    m_mprisController.pause();
}

void PlayerAdaptor::PlayPause()
{
    m_mprisController.playpause();
}

void PlayerAdaptor::Stop()
{
    m_mprisController.stop();
}

void PlayerAdaptor::Play()
{
    m_mprisController.play();
}

void PlayerAdaptor::Seek(qlonglong offset)
{
    m_mprisController.seek(offset);
}

void PlayerAdaptor::SetPosition(const QDBusObjectPath &trackId, qlonglong position)
{
    Q_UNUSED(trackId)
    m_mprisController.seekTo(position);
}

void PlayerAdaptor::OpenUri(const QString &uri)
{
    m_mprisController.openUri(uri);
}

#include "moc_playeradaptor.cpp"
