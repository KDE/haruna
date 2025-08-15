/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef PLAYLISTMODEL_H
#define PLAYLISTMODEL_H

#include <QAbstractListModel>
#include <QSortFilterProxyModel>
#include <QUrl>
#include <QtQml/qqmlregistration.h>

#include "youtube.h"

struct YTVideoInfo;

struct PlaylistItem {
    QUrl url;
    QString filename;
    QString mediaTitle;
    QString folderPath;
    QString formattedDuration;
    double duration{0.0};
};

class PlaylistProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
    QML_NAMED_ELEMENT(PlaylistProxyModel)

public:
    explicit PlaylistProxyModel(QObject *parent = nullptr);

    enum class Sort {
        NameAscending,
        NameDescending,
        DurationAscending,
        DurationDescending,
    };
    Q_ENUM(Sort)

    Q_INVOKABLE void sortItems(Sort sortMode);
    Q_INVOKABLE uint getPlayingItem();
    Q_INVOKABLE void setPlayingItem(uint i);
    Q_INVOKABLE void playNext();
    Q_INVOKABLE void playPrevious();
    Q_INVOKABLE void saveM3uFile(const QString &path);
    Q_INVOKABLE void highlightInFileManager(uint row);
    Q_INVOKABLE void removeItem(uint row);
    Q_INVOKABLE void renameFile(uint row);
    Q_INVOKABLE void trashFile(uint row);
    Q_INVOKABLE void copyFileName(uint row);
    Q_INVOKABLE void copyFilePath(uint row);
    Q_INVOKABLE QString getFilePath(uint row);
    Q_INVOKABLE bool isLastItem(uint row);
};

class PlaylistModel : public QAbstractListModel
{
    Q_OBJECT
    QML_NAMED_ELEMENT(PlaylistModel)

public:
    explicit PlaylistModel(QObject *parent = nullptr);
    friend class PlaylistProxyModel;
    friend class MpvItem;

    enum Roles {
        NameRole = Qt::UserRole,
        TitleRole,
        DurationRole,
        PathRole,
        FolderPathRole,
        PlayingRole,
        IsLocalRole,
    };
    Q_ENUM(Roles)

    enum Behaviour {
        Append,
        AppendAndPlay,
        Clear,
    };
    Q_ENUM(Behaviour)

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void clear();
    Q_INVOKABLE void addItem(const QString &path, PlaylistModel::Behaviour behaviour);
    Q_INVOKABLE void addItem(const QUrl &url, PlaylistModel::Behaviour behaviour);

Q_SIGNALS:
    void itemAdded(uint index, const QString &path);
    void playingItemChanged();

private:
    void appendItem(const QUrl &url);
    void getSiblingItems(const QUrl &url);
    void addM3uItems(const QUrl &url, PlaylistModel::Behaviour behaviour);
    void addYouTubePlaylist(QJsonArray playlist, const QString &videoId, const QString &playlistId);
    void updateFileInfo(YTVideoInfo info, QVariantMap data);
    bool isVideoOrAudioMimeType(const QString &mimeType);
    void setPlayingItem(uint i);

    std::vector<PlaylistItem> m_playlist;
    uint m_playingItem{0};
    QString m_playlistPath;
    int m_httpItemCounter{0};
    YouTube youtube;

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
    bool isShuffleOn() const;
    // when shuffling the playing item is moved to the front
    void shuffleIndexes();
    std::vector<int> shuffledIndexes() const;
    int currentShuffledIndex() const;
    void setCurrentShuffledIndex(int shuffledIndex);

    bool m_isShuffleOn{false};
    std::vector<int> m_shuffledIndexes;
    int m_currentShuffledIndex{-1};
};

Q_DECLARE_METATYPE(PlaylistModel::Behaviour)
#endif // PLAYLISTMODEL_H
