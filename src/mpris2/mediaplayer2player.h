/*
 * SPDX-FileCopyrightText: 2021 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef MEDIAPLAYER2PLAYER_H
#define MEDIAPLAYER2PLAYER_H

#include <QDBusAbstractAdaptor>
#include <QImage>

class QDBusObjectPath;
class MpvItem;

class MediaPlayer2Player : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.mpris.MediaPlayer2.Player")

public:
    explicit MediaPlayer2Player(QObject *parent = nullptr);
    ~MediaPlayer2Player() = default;

    Q_PROPERTY(QString PlaybackStatus READ PlaybackStatus NOTIFY playbackStatusChanged)
    Q_PROPERTY(QVariantMap Metadata READ Metadata NOTIFY metadataChanged)
    Q_PROPERTY(double Volume READ Volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(qlonglong Position READ Position WRITE setPosition NOTIFY playbackStatusChanged)
    Q_PROPERTY(bool CanGoNext READ CanGoNext CONSTANT)
    Q_PROPERTY(bool CanGoPrevious READ CanGoPrevious CONSTANT)
    Q_PROPERTY(bool CanPlay READ CanPlay CONSTANT)
    Q_PROPERTY(bool CanPause READ CanPause CONSTANT)
    Q_PROPERTY(bool CanSeek READ CanSeek CONSTANT)
    Q_PROPERTY(bool CanControl READ CanControl CONSTANT)

    void propertiesChanged(const QString &property, const QVariant &value);

public Q_SLOTS:
    void Next();
    void Previous();
    void Pause();
    void PlayPause();
    void Stop();
    void Play();
    void Seek(qlonglong offset);
    void SetPosition(const QDBusObjectPath &trackId, qlonglong pos);
    void OpenUri(const QString &uri);
    QString PlaybackStatus();
    QVariantMap Metadata();
    double Volume();
    qlonglong Position();
    bool CanGoNext();
    bool CanGoPrevious();
    bool CanPlay();
    bool CanPause();
    bool CanSeek();
    bool CanControl();
    void setPosition(int pos);
    void setVolume(double vol);

Q_SIGNALS:
    void next();
    void previous();
    void pause();
    void playpause();
    void stop();
    void play();
    void seek(int offset);
    void openUri(QString uri);
    void playbackStatusChanged();
    void metadataChanged();
    void volumeChanged();
    void requestMprisThumbnail(const QString &path, int width);

private:
    QString getThumbnail(const QString &path);
    MpvItem *m_mpv;
    QImage m_image;
    QVariantMap m_metadata;
};

#endif // MEDIAPLAYER2PLAYER_H
