/*
 * SPDX-FileCopyrightText: 2026 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef MPRISCONTROLLER_H
#define MPRISCONTROLLER_H

#include <QObject>

class MprisControllerPrivate;

/*!
 * \class MprisController
 * \inmodule MPRIS2
 * \brief Controls communication between application and MPRIS clients.
 *
 * MprisController receives requests from the MPRIS D-Bus adaptors and
 * synchronizes state between the application and connected MPRIS clients.
 */
class MprisController : public QObject
{
    Q_OBJECT

public:
    explicit MprisController(QObject *parent = nullptr);
    ~MprisController();

    bool canQuit() const;
    void setCanQuit(bool newCanQuit);

    bool fullscreen() const;
    void setFullscreen(bool newFullscreen);
    void requestFullscreenChange(bool newFullscreen);

    bool canSetFullscreen() const;
    void setCanSetFullscreen(bool newCanSetFullscreen);

    bool canRaise() const;
    void setCanRaise(bool newCanRaise);

    bool hasTrackList() const;
    void setHasTrackList(bool newHasTrackList);

    QString identity() const;
    void setIdentity(const QString &newIdentity);

    QString desktopEntry() const;
    void setDesktopEntry(const QString &newDesktopEntry);

    QStringList supportedUriSchemes() const;
    void setSupportedUriSchemes(const QStringList &newSupportedUriSchemes);

    QStringList supportedMimeTypes() const;
    void setSupportedMimeTypes(const QStringList &newSupportedMimeTypes);

    void raise();
    void quit();

    void next();
    void previous();
    void pause();
    void playpause();
    void stop();
    void play();
    /*!
     * \a offset in microseconds to seek by
     */
    void seek(qlonglong offset);
    /*!
     * \a position to seek to in microseconds
     */
    void seekTo(qlonglong position);
    void openUri(QString uri);

    QString playbackStatus() const;
    void setPlaybackStatus(const QString &newStatus);

    QString loopStatus() const;
    /*!
     * Set the internal loop status property to \c newLoopStatus and notify MPRIS clients.
     *
     * Should be called when loop status changes in the application.
     */
    void setLoopStatus(const QString &newLoopStatus);
    /*!
     * Request the application to change the loop status property to \c newLoopStatus
     */
    void requestLoopStatusChange(const QString &newLoopStatus);

    double rate() const;
    /*!
     * Set the internal rate property to \c newRate and notify MPRIS clients.
     *
     * Should be called when rate changes in the application.
     */
    void setRate(const double newRate);
    /*!
     * Request the application to change the rate property to \c newLoopStatus
     */
    void requestRateChange(double newRate);

    bool shuffle() const;
    /*!
     * Set the internal shuffle property to \c newShuffle and notify MPRIS clients.
     *
     * Should be called when shuffle changes in the application.
     */
    void setShuffle(const bool newShuffle);
    /*!
     * Request the application to change the shuffle property to \c newShuffle
     */
    void requestShuffleChange(bool newShuffle);

    QVariantMap metadata() const;
    void setMetadata(const QString &field, const QVariant value);

    double volume() const;
    /*!
     * Set the internal volume property to \c newVolume and notify MPRIS clients.
     *
     * Should be called when volume changes in the application.
     */
    void setVolume(const double newVolume);
    /*!
     * Request the application to change the volume property to \c newVolume
     */
    void requestVolumeChange(const double &newVolume);

    /*!
     * Returns \c position in microseconds
     */
    qlonglong position() const;
    /*!
     * Set position to /c newPosition in microseconds
     */
    void setPosition(const qlonglong newPosition);

    double minimumRate() const;
    void setMinimumRate(const double newMinRate);

    double maximumRate() const;
    void setMaximumRate(const double newMaxRate);

    bool canGoNext() const;
    void setCanGoNext(const bool newCanGoNext);
    bool canGoPrevious() const;
    void setCanGoPrevious(const bool newCanGoPrevious);
    bool canPlay() const;
    void setCanPlay(const bool newCanPlay);
    bool canPause() const;
    void setCanPause(const bool newCanPause);
    bool canSeek() const;
    void setCanSeek(const bool newCanSeek);
    bool canControl() const;
    void setCanControl(const bool newCanControl);

    /*!
     * Indicates that position has changed on the application side.
     * Causes the Seeked signal of the M ediaPlayer2.Player interface to be emitted
     */
    void seeked(qlonglong position);

Q_SIGNALS:
    void raiseRequested();
    void quitRequested();
    void nextRequested();
    void previousRequested();
    void pauseRequested();
    void playpauseRequested();
    void stopRequested();
    void playRequested();

    /*!
     * \a offset in microseconds to seek by
     */
    void seekRequested(qlonglong offset);
    /*!
     * \a position to seek to in microseconds
     */
    void seekToRequested(qlonglong position);
    void openUriRequested(QString uri);

    /*!
     * Emitted when fullscreen is changed by MPRIS client
     */
    void fullscreenChangeRequested(bool fullscreen);

    /*!
     * Emitted when volume is changed by MPRIS client
     */
    void volumeChangeRequested(double volume);

    /*!
     * Emitted when loop status is changed by MPRIS client
     */
    void loopStatusChangeRequested(const QString &loopStatus);

    /*!
     * Emitted when shuffle is changed by MPRIS client
     */
    void shuffleChangeRequested(bool shuffle);

    /*!
     * Emitted when rate is changed by MPRIS client
     */
    void rateChangeRequested(double rate);

private:
    std::unique_ptr<MprisControllerPrivate> d_ptr;
};

#endif // MPRISCONTROLLER_H
