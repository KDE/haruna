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
    Q_INVOKABLE int getPlayingItem();
    Q_INVOKABLE void setPlayingItem(int i);
    Q_INVOKABLE void playNext();
    Q_INVOKABLE void playPrevious();
    Q_INVOKABLE void saveM3uFile(const QString &path);
    Q_INVOKABLE void highlightInFileManager(int row);
    Q_INVOKABLE void removeItem(int row);
    Q_INVOKABLE void renameFile(int row);
    Q_INVOKABLE void trashFile(int row);
    Q_INVOKABLE void copyFileName(int row);
    Q_INVOKABLE void copyFilePath(int row);
    Q_INVOKABLE QString getFilePath(int row);
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
    void itemAdded(int index, const QString &path);
    void playingItemChanged();

private:
    void appendItem(const QUrl &url);
    void getSiblingItems(const QUrl &url);
    void addM3uItems(const QUrl &url);
    void getYouTubePlaylist(const QUrl &url, PlaylistModel::Behaviour behaviour);
    void getHttpItemInfo(const QUrl &url, int row);
    bool isVideoOrAudioMimeType(const QString &mimeType);
    void setPlayingItem(int i);

    QList<PlaylistItem> m_playlist;
    int m_playingItem{-1};
    QString m_playlistPath;
    int m_httpItemCounter{0};
};

Q_DECLARE_METATYPE(PlaylistModel::Behaviour)
#endif // PLAYLISTMODEL_H
