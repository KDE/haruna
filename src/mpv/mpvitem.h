/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef MPVOBJECT_H
#define MPVOBJECT_H

#include "mpvabstractitem.h"
#include "playlistmodel.h"
#include "track.h"
#include "tracksmodel.h"

class MpvRenderer;

class MpvItem : public MpvAbstractItem
{
    Q_OBJECT
    Q_PROPERTY(TracksModel *audioTracksModel READ audioTracksModel NOTIFY audioTracksModelChanged)
    Q_PROPERTY(TracksModel *subtitleTracksModel READ subtitleTracksModel NOTIFY subtitleTracksModelChanged)
    Q_PROPERTY(PlayListModel *playlistModel READ playlistModel WRITE setPlaylistModel NOTIFY playlistModelChanged)
    Q_PROPERTY(PlayListProxyModel *playlistProxyModel READ playlistProxyModel WRITE setPlaylistProxyModel NOTIFY playlistProxyModelChanged)
    // when playlist repeat is turned off the last file in the playlist is reloaded
    // this property is used to pause the player
    Q_PROPERTY(bool isFileReloaded READ isFileReloaded WRITE setIsFileReloaded NOTIFY isFileReloadedChanged)
    Q_PROPERTY(QString mediaTitle READ mediaTitle NOTIFY mediaTitleChanged)
    Q_PROPERTY(double position READ position WRITE setPosition NOTIFY positionChanged)
    // cache the watch later time position to be used by the seekToWatchLaterPosition action
    // useful when resuming playback is disabled
    Q_PROPERTY(double watchLaterPosition READ watchLaterPosition WRITE setWatchLaterPosition NOTIFY watchLaterPositionChanged)
    Q_PROPERTY(double duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(double remaining READ remaining NOTIFY remainingChanged)
    Q_PROPERTY(QString formattedPosition READ formattedPosition NOTIFY positionChanged)
    Q_PROPERTY(QString formattedDuration READ formattedDuration NOTIFY durationChanged)
    Q_PROPERTY(QString formattedRemaining READ formattedRemaining NOTIFY remainingChanged)
    Q_PROPERTY(double watchPercentage MEMBER m_watchPercentage READ watchPercentage WRITE setWatchPercentage NOTIFY watchPercentageChanged)
    Q_PROPERTY(bool pause READ pause WRITE setPause NOTIFY pauseChanged)
    Q_PROPERTY(bool mute READ mute WRITE setMute NOTIFY muteChanged)
    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(int chapter READ chapter WRITE setChapter NOTIFY chapterChanged)
    Q_PROPERTY(int audioId READ audioId WRITE setAudioId NOTIFY audioIdChanged)
    Q_PROPERTY(int subtitleId READ subtitleId WRITE setSubtitleId NOTIFY subtitleIdChanged)
    Q_PROPERTY(int secondarySubtitleId READ secondarySubtitleId WRITE setSecondarySubtitleId NOTIFY secondarySubtitleIdChanged)

    PlayListProxyModel *playlistProxyModel();
    void setPlaylistProxyModel(PlayListProxyModel *model);
    PlayListModel *playlistModel();
    void setPlaylistModel(PlayListModel *model);

    bool isFileReloaded() const;
    void setIsFileReloaded(bool _isFileReloaded);

    QString mediaTitle();

    double position();
    void setPosition(double value);

    double watchLaterPosition() const;
    void setWatchLaterPosition(double _watchLaterPosition);

    double duration();

    double remaining();

    double watchPercentage();
    void setWatchPercentage(double value);

    bool pause();
    void setPause(bool value);

    bool mute();
    void setMute(bool value);

    int volume();
    void setVolume(int value);

    int chapter();
    void setChapter(int value);

    int audioId();
    void setAudioId(int value);

    int subtitleId();
    void setSubtitleId(int value);

    int secondarySubtitleId();
    void setSecondarySubtitleId(int value);

    void onPropertyChanged(const QString &property, const QVariant &value);
    void cachePropertyValue(const QString &property, const QVariant &value);

public:
    explicit MpvItem(QQuickItem *parent = nullptr);
    ~MpvItem() = default;

    Q_INVOKABLE void loadFile(const QString &file);
    Q_INVOKABLE void saveTimePosition();
    Q_INVOKABLE double loadTimePosition();
    Q_INVOKABLE void resetTimePosition();
    Q_INVOKABLE void userCommand(const QString &commandString);

    Q_INVOKABLE QString formattedPosition() const;
    Q_INVOKABLE QString formattedRemaining() const;
    Q_INVOKABLE QString formattedDuration() const;
    Q_INVOKABLE QVariant getCachedPropertyValue(const QString &property);

Q_SIGNALS:
    void audioTracksModelChanged();
    void subtitleTracksModelChanged();
    void playlistModelChanged();
    void playlistProxyModelChanged();
    void playlistTitleChanged();
    void playlistUrlChanged();
    void isFileReloadedChanged();
    void mediaTitleChanged();
    void watchPercentageChanged();
    void positionChanged();
    void watchLaterPositionChanged();
    void durationChanged();
    void remainingChanged();
    void pauseChanged();
    void muteChanged();
    void volumeChanged();
    void chapterChanged();
    void audioIdChanged();
    void subtitleIdChanged();
    void secondarySubtitleIdChanged();
    void fileStarted();
    void fileLoaded();
    void endFile(QString reason);
    void syncConfigValue(QString path, QString group, QString key, QVariant value);

    // signals used for mpris
    void raise();
    void playNext();
    void playPrevious();
    void openUri(const QString &uri);

private:
    TracksModel *audioTracksModel() const;
    TracksModel *subtitleTracksModel() const;
    void initProperties();
    void setupConnections();
    void loadTracks();
    QString md5(const QString &str);
    TracksModel *m_audioTracksModel;
    TracksModel *m_subtitleTracksModel;
    QMap<int, Track *> m_subtitleTracks;
    QMap<int, Track *> m_audioTracks;
    QList<int> m_secondsWatched;
    double m_watchPercentage{0.0};
    PlayListModel *m_playlistModel;
    PlayListProxyModel *m_playlistProxyModel;
    QString m_file;

    double m_position{0.0};
    QString m_formattedPosition;
    double m_remaining{0.0};
    QString m_formattedRemaining;
    double m_duration{0.0};
    QString m_formattedDuration;

    QMap<QString, QVariant> m_propertiesCache;
    double m_watchLaterPosition{0.0};
    bool m_isFileReloaded{false};
};

#endif // MPVOBJECT_H
