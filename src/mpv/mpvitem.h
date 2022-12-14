/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef MPVOBJECT_H
#define MPVOBJECT_H

#include "mpvcore.h"
#include "playlistmodel.h"
#include "tracksmodel.h"
#include "track.h"

class MpvRenderer;

class MpvItem : public MpvCore
{
    Q_OBJECT

    Q_PROPERTY(TracksModel* audioTracksModel READ audioTracksModel NOTIFY audioTracksModelChanged)
    Q_PROPERTY(TracksModel* subtitleTracksModel READ subtitleTracksModel NOTIFY subtitleTracksModelChanged)
    Q_PROPERTY(PlayListModel* playlistModel READ playlistModel WRITE setPlaylistModel NOTIFY playlistModelChanged)
    Q_PROPERTY(PlayListProxyModel* playlistProxyModel READ playlistProxyModel WRITE setPlaylistProxyModel NOTIFY playlistProxyModelChanged)
    Q_PROPERTY(QString mediaTitle READ mediaTitle NOTIFY mediaTitleChanged)
    Q_PROPERTY(double position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(double duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(double remaining READ remaining NOTIFY remainingChanged)
    Q_PROPERTY(double watchPercentage MEMBER m_watchPercentage READ watchPercentage WRITE setWatchPercentage NOTIFY watchPercentageChanged)
    Q_PROPERTY(bool pause READ pause WRITE setPause NOTIFY pauseChanged)
    Q_PROPERTY(bool mute READ mute WRITE setMute NOTIFY muteChanged)
    Q_PROPERTY(bool hwDecoding READ hwDecoding WRITE setHWDecoding NOTIFY hwDecodingChanged)
    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(int chapter READ chapter WRITE setChapter NOTIFY chapterChanged)
    Q_PROPERTY(int audioId READ audioId WRITE setAudioId NOTIFY audioIdChanged)
    Q_PROPERTY(int subtitleId READ subtitleId WRITE setSubtitleId NOTIFY subtitleIdChanged)
    Q_PROPERTY(int secondarySubtitleId READ secondarySubtitleId WRITE setSecondarySubtitleId NOTIFY secondarySubtitleIdChanged)

    PlayListProxyModel *playlistProxyModel();
    void setPlaylistProxyModel(PlayListProxyModel *model);
    PlayListModel *playlistModel();
    void setPlaylistModel(PlayListModel *model);


    QString mediaTitle();

    double position();
    void setPosition(double value);

    double duration();

    double remaining();

    double watchPercentage();
    void setWatchPercentage(double value);

    bool pause();
    void setPause(bool value);

    bool mute();
    void setMute(bool value);

    bool hwDecoding();
    void setHWDecoding(bool value);

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


public:
    MpvItem(QQuickItem * parent = nullptr);
    ~MpvItem() = default;

    Q_INVOKABLE void loadFile(const QString &file);
    Q_INVOKABLE void saveTimePosition();
    Q_INVOKABLE double loadTimePosition();
    Q_INVOKABLE void resetTimePosition();
    Q_INVOKABLE void userCommand(const QString &commandString);
    void eventHandler() override;

Q_SIGNALS:
    void audioTracksModelChanged();
    void subtitleTracksModelChanged();
    void playlistModelChanged();
    void playlistProxyModelChanged();
    void playlistTitleChanged();
    void playlistUrlChanged();
    void mediaTitleChanged();
    void watchPercentageChanged();
    void positionChanged();
    void durationChanged();
    void remainingChanged();
    void pauseChanged();
    void muteChanged();
    void hwDecodingChanged();
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
    void playNext();
    void playPrevious();
    void openUri(const QString &uri);

private:
    TracksModel *audioTracksModel() const;
    TracksModel *subtitleTracksModel() const;
    void initProperties();
    void loadTracks();
    QString md5(const QString &str);
    TracksModel *m_audioTracksModel;
    TracksModel *m_subtitleTracksModel;
    QMap<int, Track*> m_subtitleTracks;
    QMap<int, Track*> m_audioTracks;
    QList<int> m_secondsWatched;
    double m_watchPercentage;
    PlayListModel *m_playlistModel;
    PlayListProxyModel *m_playlistProxyModel;
    QString m_file;
};

#endif // MPVOBJECT_H
