/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef MPVOBJECT_H
#define MPVOBJECT_H

#include <QtQuick/QQuickFramebufferObject>

#include "mpvcore.h"
#include "playlistmodel.h"
#include "tracksmodel.h"

class MpvRenderer;
class Track;

class MpvItem : public MpvCore
{
    Q_OBJECT
    Q_PROPERTY(TracksModel* audioTracksModel READ audioTracksModel NOTIFY audioTracksModelChanged)
    Q_PROPERTY(TracksModel* subtitleTracksModel READ subtitleTracksModel NOTIFY subtitleTracksModelChanged)

    Q_PROPERTY(QString mediaTitle
               READ mediaTitle
               NOTIFY mediaTitleChanged)

    Q_PROPERTY(double position
               READ position
               WRITE setPosition
               NOTIFY positionChanged)

    Q_PROPERTY(double duration
               READ duration
               NOTIFY durationChanged)

    Q_PROPERTY(double remaining
               READ remaining
               NOTIFY remainingChanged)

    Q_PROPERTY(bool pause
               READ pause
               WRITE setPause
               NOTIFY pauseChanged)

    Q_PROPERTY(int volume
               READ volume
               WRITE setVolume
               NOTIFY volumeChanged)

    Q_PROPERTY(bool mute READ mute WRITE setMute NOTIFY muteChanged)

    Q_PROPERTY(int chapter
               READ chapter
               WRITE setChapter
               NOTIFY chapterChanged)

    Q_PROPERTY(int audioId
               READ audioId
               WRITE setAudioId
               NOTIFY audioIdChanged)

    Q_PROPERTY(int subtitleId
               READ subtitleId
               WRITE setSubtitleId
               NOTIFY subtitleIdChanged)

    Q_PROPERTY(int secondarySubtitleId
               READ secondarySubtitleId
               WRITE setSecondarySubtitleId
               NOTIFY secondarySubtitleIdChanged)

    Q_PROPERTY(int contrast
               READ contrast
               WRITE setContrast
               NOTIFY contrastChanged)

    Q_PROPERTY(int brightness
               READ brightness
               WRITE setBrightness
               NOTIFY brightnessChanged)

    Q_PROPERTY(int gamma
               READ gamma
               WRITE setGamma
               NOTIFY gammaChanged)

    Q_PROPERTY(int saturation
               READ saturation
               WRITE setSaturation
               NOTIFY saturationChanged)

    Q_PROPERTY(double watchPercentage
               MEMBER m_watchPercentage
               READ watchPercentage
               WRITE setWatchPercentage
               NOTIFY watchPercentageChanged)

    Q_PROPERTY(bool hwDecoding
               READ hwDecoding
               WRITE setHWDecoding
               NOTIFY hwDecodingChanged)

    Q_PROPERTY(PlayListModel* playlistModel
               READ playlistModel
               WRITE setPlaylistModel
               NOTIFY playlistModelChanged)

    PlayListModel *playlistModel();
    void setPlaylistModel(PlayListModel *model);

    Q_PROPERTY(QString playlistTitle
               READ playlistTitle
               WRITE setPlaylistTitle
               NOTIFY playlistTitleChanged)

    QString playlistTitle();
    void setPlaylistTitle(const QString &title);

    Q_PROPERTY(QString playlistUrl READ playlistUrl WRITE setPlaylistUrl NOTIFY playlistUrlChanged)
    const QString &playlistUrl() const;
    void setPlaylistUrl(const QString &_playlistUrl);

    QString mediaTitle();

    double position();
    void setPosition(double value);

    double remaining();
    double duration();

    bool pause();
    void setPause(bool value);
    
    int volume();
    void setVolume(int value);

    bool mute();
    void setMute(bool value);

    int chapter();
    void setChapter(int value);

    int audioId();
    void setAudioId(int value);

    int subtitleId();
    void setSubtitleId(int value);

    int secondarySubtitleId();
    void setSecondarySubtitleId(int value);

    int contrast();
    void setContrast(int value);

    int brightness();
    void setBrightness(int value);

    int gamma();
    void setGamma(int value);

    int saturation();
    void setSaturation(int value);

    double watchPercentage();
    void setWatchPercentage(double value);

    bool hwDecoding();
    void setHWDecoding(bool value);

public:
    MpvItem(QQuickItem * parent = nullptr);
    ~MpvItem() = default;

    Q_INVOKABLE void loadFile(const QString &file, bool updateLastPlayedFile = true);
    Q_INVOKABLE void getYouTubePlaylist(const QString &path);
    Q_INVOKABLE void saveTimePosition();
    Q_INVOKABLE double loadTimePosition();
    Q_INVOKABLE void resetTimePosition();
    Q_INVOKABLE void userCommand(const QString &commandString);
    void eventHandler() override;

signals:
    void mediaTitleChanged();
    void positionChanged();
    void durationChanged();
    void remainingChanged();
    void volumeChanged();
    void muteChanged();
    void pauseChanged();
    void chapterChanged();
    void audioIdChanged();
    void subtitleIdChanged();
    void secondarySubtitleIdChanged();
    void contrastChanged();
    void brightnessChanged();
    void gammaChanged();
    void saturationChanged();
    void fileStarted();
    void fileLoaded();
    void endFile(QString reason);
    void watchPercentageChanged();
    void audioTracksModelChanged();
    void subtitleTracksModelChanged();
    void hwDecodingChanged();
    void playlistModelChanged();
    void playlistTitleChanged();
    void youtubePlaylistLoaded();
    void syncConfigValue(QString path, QString group, QString key, QVariant value);

    void playlistUrlChanged();

private:
    TracksModel *audioTracksModel() const;
    TracksModel *subtitleTracksModel() const;
    TracksModel *m_audioTracksModel;
    TracksModel *m_subtitleTracksModel;
    QMap<int, Track*> m_subtitleTracks;
    QMap<int, Track*> m_audioTracks;
    QList<int> m_secondsWatched;
    double m_watchPercentage;
    PlayListModel *m_playlistModel;
    QString m_playlistTitle;
    QString m_playlistUrl;
    QString m_file;

    void initProperties();
    void loadTracks();
    QString md5(const QString &str);
    int m_mute;
};

#endif // MPVOBJECT_H
