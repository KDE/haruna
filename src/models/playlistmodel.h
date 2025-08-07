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
    void addM3uItems(const QUrl &url);
    void addYouTubePlaylist(QJsonArray playlist, const QString &videoId, const QString &playlistId);
    void updateFileInfo(YTVideoInfo info, QVariantMap data);
    bool isVideoOrAudioMimeType(const QString &mimeType);
    void setPlayingItem(uint i);

    QList<PlaylistItem> m_playlist;
    uint m_playingItem{0};
    QString m_playlistPath;
    int m_httpItemCounter{0};
    YouTube youtube;
};

Q_DECLARE_METATYPE(PlaylistModel::Behaviour)
#endif // PLAYLISTMODEL_H
