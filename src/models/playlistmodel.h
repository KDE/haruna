/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef PLAYLISTMODEL_H
#define PLAYLISTMODEL_H

#include <QAbstractTableModel>
#include <KSharedConfig>
#include <map>
#include <memory>

class PlayListItem;

using Playlist = QList<PlayListItem*>;

class PlayListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int playingVideo
               MEMBER m_playingVideo
               READ getPlayingVideo
               WRITE setPlayingVideo
               NOTIFY playingVideoChanged)

public:
    explicit PlayListModel(QObject *parent = nullptr);

    enum {
        NameRole = Qt::UserRole,
        TitleRole,
        DurationRole,
        PathRole,
        FolderPathRole,
        PlayingRole
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE QString getPath(int i);
    Q_INVOKABLE PlayListItem *getItem(int i);
    Q_INVOKABLE void setPlayingVideo(int playingVideo);
    Q_INVOKABLE int getPlayingVideo() const;
    Q_INVOKABLE void getVideos(QString path);
    Q_INVOKABLE void clear();

    Playlist getPlayList() const;
    void setPlayList(const Playlist &playList);

signals:
    void videoAdded(int index, QString path);
    void playingVideoChanged();

private:
    Playlist items() const;
    QString configFolder();
    Playlist m_playList;
    int m_playingVideo = 0;
    KSharedConfig::Ptr m_config;
};

#endif // PLAYLISTMODEL_H
