/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef PLAYLISTMODEL_H
#define PLAYLISTMODEL_H

#include <QAbstractListModel>
#include <QThreadPool>
#include <QUrl>
#include <QtQml/qqmlregistration.h>

#include <KFileMetaData/Properties>

#include "playlistmetadata.h"
#include "youtube.h"

struct YTVideoInfo;
struct AudioMetaData;
struct VideoMetaData;

struct PlaylistItem {
    QUrl url;
    QString filename;
    QString mediaTitle;
    QString folderPath;
    QString dirName;
    QString formattedDuration;
    double duration{0.0};
    qint64 fileSize{0};
    QDateTime modifiedDate;
    QString extension;
    QString fileType;

    AudioMetaData audio;
    VideoMetaData video;
};

class PlaylistModel : public QAbstractListModel
{
    Q_OBJECT
    QML_NAMED_ELEMENT(PlaylistModel)

public:
    explicit PlaylistModel(QObject *parent = nullptr);
    ~PlaylistModel();
    friend class PlaylistMultiProxiesModel;
    friend class PlaylistFilterProxyModel;
    friend class PlaylistProxyModel;
    friend class PlaylistSortProxyModel;
    friend class MpvItem;

    enum Roles {
        NameRole = Qt::UserRole,
        // General
        DurationRole,
        PathRole,
        FolderPathRole,
        DirNameRole,
        DateRole,
        FileSizeRole,
        TypeRole,
        ExtensionRole,
        PlayingRole,
        IsLocalRole,
        IsSelectedRole,
        SectionRole,
        // Audio
        TrackNoRole,
        DiscNoRole,
        GenreRole,
        TitleRole,
        ReleaseYearRole,
        AlbumRole,
        ArtistRole,
        AlbumArtistRole,
        ComposerRole,
        LyricistRole,
        AudioCodecRole,
        SampleRateRole,
        BitrateRole,
        // Video
        DisplayedWidthRole,
        DisplayedHeightRole,
        FramerateRole,
        VideoCodecRole,
        OrientationRole,
    };
    Q_ENUM(Roles)

    enum Behavior {
        Append,
        AppendAndPlay,
        Insert,
        Clear,
    };
    Q_ENUM(Behavior)

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void clear();
    void addItem(const QUrl &url, PlaylistModel::Behavior behavior);
    void stop();

Q_SIGNALS:
    void itemAdded(uint index, const QString &path, QString playlistName);
    void playingItemChanged(QString playlistName);
    void metaDataReady(uint index, const QUrl &url, KFileMetaData::PropertyMultiMap metadata);

private:
    void appendItem(const QUrl &url);
    void removeItem(const uint row);
    void getSiblingItems(const QUrl &url);
    void addM3uItems(const QUrl &url, PlaylistModel::Behavior behavior);
    void addYouTubePlaylist(QJsonArray playlist, const QString &videoId, const QString &playlistId);
    void updateFileInfo(YTVideoInfo info, QVariantMap data);
    bool isVideoOrAudioMimeType(const QString &mimeType);
    void setPlayingItem(uint i);
    void getMetaData(uint i, const QString &path);
    void onMetaDataReady(uint i, const QUrl &url, KFileMetaData::PropertyMultiMap properties);

    std::vector<PlaylistItem> m_playlist;
    QString m_playlistName{u"Default"};
    // The flag to check if this is the active playlist.
    bool m_isPlaying{false};
    uint m_playingItem{0};
    QString m_playlistPath;
    int m_httpItemCounter{0};
    YouTube youtube;
    QThreadPool m_threadPool;

    // shuffling
    // when shuffling is on, instead of using an m_playlist index to determine
    // the next and previous item to play, an index (m_currentShuffledRow) of m_shuffledIndexes is used

    // m_playlist          m_shuffledIndexes
    // 0 file_0            0 m_playlist.index_4 (file_4}
    // 1 file_1            1 m_playlist.index_3 (file_3)
    // 2 file_2            2 m_playlist.index_1 (file_1)
    // 3 file_3            3 m_playlist.index_2 (file_2)
    // 4 file_4            4 m_playlist.index_0 (file_0)
    //
    // if the current file is file_3 at index m_playlist.index_3
    // when shuffle if OFF m_playlist.index_3 in incremented/decremented by 1 for the next/previous item
    // previous file: m_playlist.index_2 file_2
    //  current file: m_playlist.index_3 file_3
    //     next file: m_playlist.index_4 file_4
    //
    // when shuffle if ON m_shuffledIndexes.index_1 in incremented/decremented by 1 for the next/previous item
    // previous file: m_shuffledIndexes.index_0 -> m_playlist.index_4 file_4
    //  current file: m_shuffledIndexes.index_1 -> m_playlist.index_3 file_3
    //     next file: m_shuffledIndexes.index_2 -> m_playlist.index_1 file_1

    // when shuffling the playing item is moved to the front
    void shuffleIndexes(std::vector<int> included = std::vector<int>());
    std::vector<int> shuffledIndexes() const;
    int currentShuffledIndex() const;
    void setCurrentShuffledIndex(int shuffledIndex);

    std::vector<int> m_shuffledIndexes;
    int m_currentShuffledIndex{-1};
};

Q_DECLARE_METATYPE(PlaylistModel::Behavior)
#endif // PLAYLISTMODEL_H
