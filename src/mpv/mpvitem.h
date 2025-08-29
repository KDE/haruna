/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 * SPDX-FileCopyrightText: 2025 Muhammet Sadık Uğursoy <sadikugursoy@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef MPVOBJECT_H
#define MPVOBJECT_H

#include <MpvAbstractItem>

#include <memory>

#include "recentfilesmodel.h"

class ChaptersModel;
class PlaylistFilterProxyModel;
class PlaylistMultiProxiesModel;
class TracksModel;
class MpvRenderer;

class MpvItem : public MpvAbstractItem
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit MpvItem(QQuickItem *parent = nullptr);
    ~MpvItem();

    enum class AsyncIds {
        None,
        SavePosition,
        Screenshot,
        TrackList,
        ChapterList,
        VideoId,
        AddSubtitleTrack,
    };
    Q_ENUM(AsyncIds)

    Q_PROPERTY(PlaylistFilterProxyModel *activeFilterProxyModel READ activeFilterProxyModel NOTIFY activeFilterProxyModelChanged)
    PlaylistFilterProxyModel *activeFilterProxyModel();

    Q_PROPERTY(PlaylistFilterProxyModel *visibleFilterProxyModel READ visibleFilterProxyModel NOTIFY visibleFilterProxyModelChanged)
    PlaylistFilterProxyModel *visibleFilterProxyModel();

    Q_PROPERTY(PlaylistFilterProxyModel *defaultFilterProxyModel READ defaultFilterProxyModel NOTIFY defaultFilterProxyModelChanged)
    PlaylistFilterProxyModel *defaultFilterProxyModel();

    Q_PROPERTY(PlaylistMultiProxiesModel *playlists READ playlists WRITE setPlaylists NOTIFY playlistsChanged)
    PlaylistMultiProxiesModel *playlists();
    void setPlaylists(PlaylistMultiProxiesModel *model);

    Q_PROPERTY(TracksModel *audioTracksModel READ audioTracksModel NOTIFY audioTracksModelChanged)
    TracksModel *audioTracksModel() const;

    Q_PROPERTY(TracksModel *subtitleTracksModel READ subtitleTracksModel NOTIFY subtitleTracksModelChanged)
    TracksModel *subtitleTracksModel() const;

    Q_PROPERTY(ChaptersModel *chaptersModel READ chaptersModel WRITE setChaptersModel NOTIFY chaptersModelChanged)
    ChaptersModel *chaptersModel() const;
    void setChaptersModel(ChaptersModel *_chaptersModel);

    Q_PROPERTY(QString mediaTitle READ mediaTitle NOTIFY mediaTitleChanged)
    QString mediaTitle();

    Q_PROPERTY(double position READ position WRITE setPosition NOTIFY positionChanged)
    double position();
    void setPosition(double value);

    Q_PROPERTY(double duration READ duration NOTIFY durationChanged)
    double duration();

    Q_PROPERTY(double remaining READ remaining NOTIFY remainingChanged)
    double remaining();

    Q_PROPERTY(QString formattedPosition READ formattedPosition NOTIFY positionChanged)
    QString formattedPosition() const;

    Q_PROPERTY(QString formattedDuration READ formattedDuration NOTIFY durationChanged)
    QString formattedRemaining() const;

    Q_PROPERTY(QString formattedRemaining READ formattedRemaining NOTIFY remainingChanged)
    QString formattedDuration() const;

    /**
     * cache the watch later time position to be used by the seekToWatchLaterPosition action
     * useful when resuming playback is disabled
     */
    Q_PROPERTY(double watchLaterPosition READ watchLaterPosition WRITE setWatchLaterPosition NOTIFY watchLaterPositionChanged)
    double watchLaterPosition() const;
    void setWatchLaterPosition(double _watchLaterPosition);

    Q_PROPERTY(double watchPercentage READ watchPercentage WRITE setWatchPercentage NOTIFY watchPercentageChanged)
    double watchPercentage();
    void setWatchPercentage(double value);

    Q_PROPERTY(bool pause READ pause WRITE setPause NOTIFY pauseChanged)
    bool pause();
    void setPause(bool value);

    Q_PROPERTY(bool mute READ mute WRITE setMute NOTIFY muteChanged)
    bool mute();
    void setMute(bool value);

    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged)
    int volume();
    void setVolume(int value);

    Q_PROPERTY(int volumeMax READ volumeMax WRITE setVolumeMax NOTIFY volumeMaxChanged)
    int volumeMax();
    void setVolumeMax(int value);

    Q_PROPERTY(int chapter READ chapter WRITE setChapter NOTIFY chapterChanged)
    int chapter();
    void setChapter(int value);

    Q_PROPERTY(int audioId READ audioId WRITE setAudioId NOTIFY audioIdChanged)
    int audioId();
    void setAudioId(int value);

    Q_PROPERTY(int subtitleId READ subtitleId WRITE setSubtitleId NOTIFY subtitleIdChanged)
    int subtitleId();
    void setSubtitleId(int value);

    Q_PROPERTY(int secondarySubtitleId READ secondarySubtitleId WRITE setSecondarySubtitleId NOTIFY secondarySubtitleIdChanged)
    int secondarySubtitleId();
    void setSecondarySubtitleId(int value);

    Q_PROPERTY(int videoWidth READ videoWidth NOTIFY videoWidthChanged)
    int videoWidth();

    Q_PROPERTY(int videoHeight READ videoHeight NOTIFY videoHeightChanged)
    int videoHeight();

    /**
     * Whether the file finished loading and it's safe to start certain events,
     * like auto skipping chapters, which otherwise skips wrong chapter due to the multithreaded code
     */
    Q_PROPERTY(bool finishedLoading READ finishedLoading WRITE setFinishedLoading NOTIFY finishedLoadingChanged)
    bool finishedLoading() const;
    void setFinishedLoading(bool _finishedLoading);

    Q_PROPERTY(QUrl currentUrl READ currentUrl NOTIFY currentUrlChanged)
    QUrl currentUrl() const;

    Q_PROPERTY(bool isReady READ isReady WRITE setIsReady NOTIFY isReadyChanged)
    bool isReady() const;
    void setIsReady(bool _isReady);

    Q_INVOKABLE void loadFile(const QString &file);
    Q_INVOKABLE void userCommand(const QString &commandString);
    Q_INVOKABLE void selectSubtitleTrack();

Q_SIGNALS:
    void audioTracksModelChanged();
    void subtitleTracksModelChanged();
    void activeFilterProxyModelChanged();
    void visibleFilterProxyModelChanged();
    void defaultFilterProxyModelChanged();
    void playlistsChanged();
    void chaptersModelChanged();
    void finishedLoadingChanged();
    void playlistTitleChanged();
    void playlistUrlChanged();
    void mediaTitleChanged();
    void currentUrlChanged();
    void watchPercentageChanged();
    void positionChanged();
    void watchLaterPositionChanged();
    void durationChanged();
    void remainingChanged();
    void pauseChanged();
    void muteChanged();
    void volumeChanged();
    void volumeMaxChanged();
    void chapterChanged();
    void chapterListChanged();
    void audioIdChanged();
    void subtitleIdChanged();
    void secondarySubtitleIdChanged();
    void isReadyChanged();
    void fileStarted();
    void fileLoaded();
    void videoReconfig();
    void videoWidthChanged();
    void videoHeightChanged();
    void eofReachedChanged();
    void savePositionToDB(const QString &md5Hash, const QString &path, double position);

    void osdMessage(const QString &text);
    void addToRecentFiles(const QUrl &url, RecentFilesModel::OpenedFrom from, const QString &name);

    // signals used for mpris
    void raise();
    void playNext();
    void playPrevious();
    void openUri(const QString &uri);

private:
    void initProperties();
    void setupConnections();
    void onReady();
    void setupEofBehavior();
    void onEndFile(const QString &reason);
    void onEndOfFileReached();
    void onPropertyChanged(const QString &property, const QVariant &value);
    void saveTimePosition();
    double loadTimePosition();
    void resetTimePosition();
    void loadTracks(QList<QVariant> tracks);
    void onAsyncReply(const QVariant &data, mpv_event event);
    void onChapterChanged();
    QString md5(const QString &str);

    std::unique_ptr<TracksModel> m_audioTracksModel;
    std::unique_ptr<TracksModel> m_subtitleTracksModel;
    QList<int> m_secondsWatched;
    double m_watchPercentage{0.0};
    std::unique_ptr<PlaylistMultiProxiesModel> m_playlists;

    double m_position{0.0};
    QString m_formattedPosition;
    double m_remaining{0.0};
    QString m_formattedRemaining;
    double m_duration{0.0};
    QString m_formattedDuration;
    QString m_mediaTitle;
    bool m_pause{true};
    bool m_mute{false};
    int m_chapter{0};
    int m_volume{0};
    int m_volumeMax{100};
    int m_audioId{0};
    int m_subtitleId{0};
    int m_secondarySubtitleId{0};
    int m_videoWidth{0};
    int m_videoHeight{0};
    bool m_eofReached{false};
    QList<QVariant> m_chaptersList;

    double m_watchLaterPosition{0.0};
    bool m_isReady{false};
    QUrl m_currentUrl;
    std::unique_ptr<ChaptersModel> m_chaptersModel;
    bool m_finishedLoading{false};
    QString m_watchLaterPath;
    std::unique_ptr<QTimer> m_saveTimePositionTimer;
};

#endif // MPVOBJECT_H
