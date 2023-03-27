/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef PLAYLISTMODEL_H
#define PLAYLISTMODEL_H

#include <KSharedConfig>
#include <QAbstractTableModel>
#include <QSortFilterProxyModel>
#include <map>
#include <memory>

class PlayListItem;

using Playlist = QList<PlayListItem *>;

class PlayListProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit PlayListProxyModel(QObject *parent = nullptr);

    enum Sort {
        NameAscending,
        NameDescending,
        DurationAscending,
        DurationDescending,
    };
    Q_ENUM(Sort);

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
};

class PlayListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int playingItem MEMBER m_playingItem READ getPlayingItem WRITE setPlayingItem NOTIFY playingItemChanged)

public:
    explicit PlayListModel(QObject *parent = nullptr);

    enum {
        NameRole = Qt::UserRole,
        TitleRole,
        DurationRole,
        PathRole,
        FolderPathRole,
        PlayingRole,
        IsLocalRole,
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE QString getPath(int index = -1);
    Q_INVOKABLE PlayListItem *getItem(int index);
    Q_INVOKABLE void setPlayingItem(int i);
    Q_INVOKABLE int getPlayingItem() const;
    Q_INVOKABLE void appendItem(QString path);
    Q_INVOKABLE void removeItem(int index);
    Q_INVOKABLE void clear();
    Q_INVOKABLE void openM3uFile(const QString &path);
    Q_INVOKABLE void openFile(const QString &path);
    Q_INVOKABLE void getYouTubePlaylist(const QString &path);

    Playlist getPlayList();

Q_SIGNALS:
    void itemAdded(int index, QString path);
    void itemRemoved(int index, QString path);
    void playingItemChanged();
    void opened(const QString &name, const QString &path);

private:
    bool isVideoOrAudioMimeType(const QString &mimeType);
    Playlist items() const;
    void getSiblingItems(QUrl url);
    void getHttpItemInfo(const QString &url, int row);
    Playlist m_playlist;
    int m_playingItem{-1};
    bool m_emitOpened{false};
};

Q_DECLARE_METATYPE(PlayListProxyModel::Sort);
#endif // PLAYLISTMODEL_H
