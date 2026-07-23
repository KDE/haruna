/*
 * SPDX-FileCopyrightText: 2026 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "mpriscontroller.h"

#include <QDBusConnection>
#include <QTimer>

#include "playeradaptor.h"
#include "rootadaptor.h"

using namespace Qt::StringLiterals;

class MprisControllerPrivate
{
    Q_DISABLE_COPY(MprisControllerPrivate);

public:
    MprisControllerPrivate(MprisController *q)
        : q_ptr(q)
        , rootAdaptor(*q)
        , playerAdaptor(*q)
    {
    }
    MprisController *const q_ptr;

    QTimer notifyTimer;

    // Interface MediaPlayer2
    RootAdaptor rootAdaptor;
    bool canQuit = false;
    bool fullscreen = false;
    bool canSetFullscreen = false;
    bool canRaise = false;
    bool hasTrackList = false;
    QString identity;
    QString desktopEntry;
    QStringList supportedUriSchemes;
    QStringList supportedMimeTypes;

    // Interface MediaPlayer2.Player
    PlayerAdaptor playerAdaptor;
    QString playbackStatus;
    QString loopStatus;
    double rate = 1.0;
    bool shuffle = false;
    QVariantMap metadata;
    double volume = 1.0;
    qlonglong position = 0;
    double minimumRate = 0.0;
    double maximumRate = 1.0;
    bool canGoNext = false;
    bool canGoPrevious = false;
    bool canPlay = false;
    bool canPause = false;
    bool canSeek = false;
    bool canControl = false;
};

MprisController::MprisController(QObject *parent)
    : QObject{parent}
    , d_ptr{std::make_unique<MprisControllerPrivate>(this)}
{
    d_ptr->notifyTimer.setSingleShot(true);
    d_ptr->notifyTimer.setInterval(100);
    d_ptr->notifyTimer.callOnTimeout([this]() {
        d_ptr->playerAdaptor.notify(u"Metadata"_s, d_ptr->metadata);
    });

    // register mpris dbus service
    QDBusConnection::sessionBus().registerService(u"org.mpris.MediaPlayer2.haruna"_s);
    QDBusConnection::sessionBus().registerObject(u"/org/mpris/MediaPlayer2"_s, this, QDBusConnection::ExportAdaptors);
}

MprisController::~MprisController() = default;

void MprisController::raise()
{
    Q_EMIT raiseRequested();
}

void MprisController::quit()
{
    Q_EMIT quitRequested();
}

void MprisController::next()
{
    Q_EMIT nextRequested();
}

void MprisController::previous()
{
    Q_EMIT previousRequested();
}

void MprisController::pause()
{
    Q_EMIT pauseRequested();
}

void MprisController::playpause()
{
    Q_EMIT playpauseRequested();
}

void MprisController::stop()
{
    Q_EMIT stopRequested();
}

void MprisController::play()
{
    Q_EMIT playRequested();
}

void MprisController::seek(qlonglong offset)
{
    Q_EMIT seekRequested(offset);
}

void MprisController::seekTo(qlonglong position)
{
    Q_EMIT seekToRequested(position);
}

void MprisController::openUri(QString uri)
{
    Q_EMIT openUriRequested(uri);
}

bool MprisController::canQuit() const
{
    return d_ptr->canQuit;
}

void MprisController::setCanQuit(bool newCanQuit)
{
    if (d_ptr->canQuit == newCanQuit) {
        return;
    }
    d_ptr->canQuit = newCanQuit;
    d_ptr->rootAdaptor.notify(u"CanQuit"_s, d_ptr->canQuit);
}

bool MprisController::fullscreen() const
{
    return d_ptr->fullscreen;
}

void MprisController::setFullscreen(bool newFullscreen)
{
    if (d_ptr->fullscreen == newFullscreen) {
        return;
    }
    d_ptr->fullscreen = newFullscreen;
    d_ptr->rootAdaptor.notify(u"Fullscreen"_s, d_ptr->fullscreen);
}

void MprisController::requestFullscreenChange(bool newFullscreen)
{
    Q_EMIT fullscreenChangeRequested(newFullscreen);
}

bool MprisController::canSetFullscreen() const
{
    return d_ptr->canSetFullscreen;
}

void MprisController::setCanSetFullscreen(bool newCanSetFullscreen)
{
    if (d_ptr->canSetFullscreen == newCanSetFullscreen) {
        return;
    }
    d_ptr->canSetFullscreen = newCanSetFullscreen;
    d_ptr->rootAdaptor.notify(u"CanSetFullscreen"_s, d_ptr->canSetFullscreen);
}

bool MprisController::canRaise() const
{
    return d_ptr->canRaise;
}

void MprisController::setCanRaise(bool newCanRaise)
{
    if (d_ptr->canRaise == newCanRaise) {
        return;
    }
    d_ptr->canRaise = newCanRaise;
    d_ptr->rootAdaptor.notify(u"CanRaise"_s, d_ptr->canRaise);
}

bool MprisController::hasTrackList() const
{
    return d_ptr->hasTrackList;
}

void MprisController::setHasTrackList(bool newHasTrackList)
{
    if (d_ptr->hasTrackList == newHasTrackList) {
        return;
    }
    d_ptr->hasTrackList = newHasTrackList;
    d_ptr->rootAdaptor.notify(u"HasTrackList"_s, d_ptr->hasTrackList);
}

QString MprisController::identity() const
{
    return d_ptr->identity;
}

void MprisController::setIdentity(const QString &newIdentity)
{
    if (d_ptr->identity == newIdentity) {
        return;
    }
    d_ptr->identity = newIdentity;
    d_ptr->rootAdaptor.notify(u"Identity"_s, d_ptr->identity);
}

QString MprisController::desktopEntry() const
{
    return d_ptr->desktopEntry;
}

void MprisController::setDesktopEntry(const QString &newDesktopEntry)
{
    if (d_ptr->desktopEntry == newDesktopEntry) {
        return;
    }
    d_ptr->desktopEntry = newDesktopEntry;
    d_ptr->rootAdaptor.notify(u"DesktopEntry"_s, d_ptr->desktopEntry);
}

QStringList MprisController::supportedUriSchemes() const
{
    return d_ptr->supportedUriSchemes;
}

void MprisController::setSupportedUriSchemes(const QStringList &newSupportedUriSchemes)
{
    if (d_ptr->supportedUriSchemes == newSupportedUriSchemes) {
        return;
    }
    d_ptr->supportedUriSchemes = newSupportedUriSchemes;
    d_ptr->rootAdaptor.notify(u"SupportedUriSchemes"_s, d_ptr->supportedUriSchemes);
}

QStringList MprisController::supportedMimeTypes() const
{
    return d_ptr->supportedMimeTypes;
}

void MprisController::setSupportedMimeTypes(const QStringList &newSupportedMimeTypes)
{
    if (d_ptr->supportedMimeTypes == newSupportedMimeTypes) {
        return;
    }
    d_ptr->supportedMimeTypes = newSupportedMimeTypes;
    d_ptr->rootAdaptor.notify(u"SupportedMimeTypes"_s, d_ptr->supportedMimeTypes);
}

QString MprisController::playbackStatus() const
{
    return d_ptr->playbackStatus;
}

void MprisController::setPlaybackStatus(const QString &newStatus)
{
    if (d_ptr->playbackStatus == newStatus) {
        return;
    }
    d_ptr->playbackStatus = newStatus;
    d_ptr->playerAdaptor.notify(u"PlaybackStatus"_s, d_ptr->playbackStatus);
}

QString MprisController::loopStatus() const
{
    return d_ptr->loopStatus;
}

void MprisController::requestLoopStatusChange(const QString &newLoopStatus)
{
    Q_EMIT loopStatusChangeRequested(newLoopStatus);
}

double MprisController::rate() const
{
    return d_ptr->rate;
}

void MprisController::setRate(const double newRate)
{
    if (d_ptr->rate == newRate) {
        return;
    }
    d_ptr->rate = newRate;
    d_ptr->playerAdaptor.notify(u"Rate"_s, d_ptr->rate);
}

void MprisController::requestRateChange(double newRate)
{
    Q_EMIT rateChangeRequested(newRate);
}

void MprisController::setLoopStatus(const QString &newLoopStatus)
{
    if (d_ptr->loopStatus == newLoopStatus) {
        return;
    }
    d_ptr->loopStatus = newLoopStatus;
    d_ptr->playerAdaptor.notify(u"LoopStatus"_s, d_ptr->loopStatus);
}

bool MprisController::shuffle() const
{
    return d_ptr->shuffle;
}

void MprisController::setShuffle(const bool newShuffle)
{
    if (d_ptr->shuffle == newShuffle) {
        return;
    }
    d_ptr->shuffle = newShuffle;
    d_ptr->playerAdaptor.notify(u"Shuffle"_s, d_ptr->shuffle);
}

void MprisController::requestShuffleChange(bool newShuffle)
{
    Q_EMIT shuffleChangeRequested(newShuffle);
}

QVariantMap MprisController::metadata() const
{
    return d_ptr->metadata;
}

void MprisController::setMetadata(const QString &field, const QVariant value)
{
    d_ptr->metadata.insert(field, value);
    d_ptr->notifyTimer.start();
}

double MprisController::volume() const
{
    return d_ptr->volume;
}

void MprisController::setVolume(const double newVolume)
{
    if (d_ptr->volume == newVolume) {
        return;
    }
    d_ptr->volume = newVolume;
    d_ptr->playerAdaptor.notify(u"Volume"_s, d_ptr->volume);
}

void MprisController::requestVolumeChange(const double &newVolume)
{
    Q_EMIT volumeChangeRequested(newVolume);
}

qlonglong MprisController::position() const
{
    return d_ptr->position;
}

void MprisController::setPosition(const qlonglong newPosition)
{
    if (d_ptr->position == newPosition) {
        return;
    }
    d_ptr->position = newPosition;
}

double MprisController::minimumRate() const
{
    return d_ptr->minimumRate;
}

void MprisController::setMinimumRate(const double newMinRate)
{
    if (d_ptr->minimumRate == newMinRate) {
        return;
    }
    d_ptr->minimumRate = newMinRate;
    d_ptr->playerAdaptor.notify(u"MinimumRate"_s, d_ptr->minimumRate);
}

double MprisController::maximumRate() const
{
    return d_ptr->maximumRate;
}

void MprisController::setMaximumRate(const double newMaxRate)
{
    if (d_ptr->maximumRate == newMaxRate) {
        return;
    }
    d_ptr->maximumRate = newMaxRate;
    d_ptr->playerAdaptor.notify(u"MaximumRate"_s, d_ptr->maximumRate);
}

bool MprisController::canGoNext() const
{
    return d_ptr->canGoNext;
}

void MprisController::setCanGoNext(const bool newCanGoNext)
{
    if (d_ptr->canGoNext == newCanGoNext) {
        return;
    }
    d_ptr->canGoNext = newCanGoNext;
    d_ptr->playerAdaptor.notify(u"CanGoNext"_s, d_ptr->canGoNext);
}

bool MprisController::canGoPrevious() const
{
    return d_ptr->canGoPrevious;
}

void MprisController::setCanGoPrevious(const bool newCanGoPrevious)
{
    if (d_ptr->canGoPrevious == newCanGoPrevious) {
        return;
    }
    d_ptr->canGoPrevious = newCanGoPrevious;
    d_ptr->playerAdaptor.notify(u"CanGoPrevious"_s, d_ptr->canGoPrevious);
}

bool MprisController::canPlay() const
{
    return d_ptr->canPlay;
}

void MprisController::setCanPlay(const bool newCanPlay)
{
    if (d_ptr->canPlay == newCanPlay) {
        return;
    }
    d_ptr->canPlay = newCanPlay;
    d_ptr->playerAdaptor.notify(u"CanPlay"_s, d_ptr->canPlay);
}

bool MprisController::canPause() const
{
    return d_ptr->canPause;
}

void MprisController::setCanPause(const bool newCanPause)
{
    if (d_ptr->canPause == newCanPause) {
        return;
    }
    d_ptr->canPause = newCanPause;
    d_ptr->playerAdaptor.notify(u"CanPause"_s, d_ptr->canPause);
}

bool MprisController::canSeek() const
{
    return d_ptr->canSeek;
}

void MprisController::setCanSeek(const bool newCanSeek)
{
    if (d_ptr->canSeek == newCanSeek) {
        return;
    }
    d_ptr->canSeek = newCanSeek;
    d_ptr->playerAdaptor.notify(u"CanSeek"_s, d_ptr->canSeek);
}

bool MprisController::canControl() const
{
    return d_ptr->canControl;
}

void MprisController::setCanControl(const bool newCanControl)
{
    if (d_ptr->canControl == newCanControl) {
        return;
    }
    d_ptr->canControl = newCanControl;
    d_ptr->playerAdaptor.notify(u"CanControl"_s, d_ptr->canControl);
}

void MprisController::seeked(qlonglong position)
{
    d_ptr->playerAdaptor.seeked(position);
}

#include "moc_mpriscontroller.cpp"