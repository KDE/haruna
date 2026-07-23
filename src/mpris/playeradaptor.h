/*
 * SPDX-FileCopyrightText: 2021 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef MEDIAPLAYER2PLAYER_H
#define MEDIAPLAYER2PLAYER_H

#include <QDBusAbstractAdaptor>

class MprisController;
class MpvItem;
class QDBusObjectPath;
class QImage;

// https://specifications.freedesktop.org/mpris/latest/Player_Interface.html
class PlayerAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.mpris.MediaPlayer2.Player")

public:
    explicit PlayerAdaptor(MprisController &parent);
    ~PlayerAdaptor() = default;

    Q_PROPERTY(QString PlaybackStatus READ playbackStatus CONSTANT)
    QString playbackStatus();

    Q_PROPERTY(QString LoopStatus READ loopStatus WRITE setLoopStatus CONSTANT)
    QString loopStatus();
    void setLoopStatus(const QString &newLoopStatus);

    Q_PROPERTY(double Rate READ rate WRITE setRate CONSTANT)
    double rate() const;
    void setRate(double newRate);

    Q_PROPERTY(bool Shuffle READ shuffle WRITE setShuffle CONSTANT)
    bool shuffle();
    void setShuffle(bool newShuffle);

    Q_PROPERTY(QVariantMap Metadata READ metadata CONSTANT)
    QVariantMap metadata();

    Q_PROPERTY(double Volume READ volume WRITE setVolume CONSTANT)
    double volume();
    void setVolume(double newVolume);

    Q_PROPERTY(qlonglong Position READ position CONSTANT)
    qlonglong position();

    Q_PROPERTY(double MinimumRate READ minimumRate CONSTANT)
    double minimumRate() const;

    Q_PROPERTY(double MaximumRate READ maximumRate CONSTANT)
    double maximumRate() const;

    Q_PROPERTY(bool CanGoNext READ canGoNext CONSTANT)
    bool canGoNext();

    Q_PROPERTY(bool CanGoPrevious READ canGoPrevious CONSTANT)
    bool canGoPrevious();

    Q_PROPERTY(bool CanPlay READ canPlay CONSTANT)
    bool canPlay();

    Q_PROPERTY(bool CanPause READ canPause CONSTANT)
    bool canPause();

    Q_PROPERTY(bool CanSeek READ canSeek CONSTANT)
    bool canSeek();

    Q_PROPERTY(bool CanControl READ canControl CONSTANT)
    bool canControl();

    void notify(const QString &property, const QVariant &value);
    void seeked(qlonglong position);

public Q_SLOTS:
    void Next();
    void Previous();
    void Pause();
    void PlayPause();
    void Stop();
    void Play();
    void Seek(qlonglong offset);
    void SetPosition(const QDBusObjectPath &trackId, qlonglong position);
    void OpenUri(const QString &uri);

Q_SIGNALS:
    void Seeked(qlonglong position);

private:
    MprisController &m_mprisController;
};

#endif // MEDIAPLAYER2PLAYER_H
