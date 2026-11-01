/*
 * SPDX-FileCopyrightText: 2026 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef PLAYLISTSMANAGER_H
#define PLAYLISTSMANAGER_H

#include <QObject>
#include <QQmlEngine>

#include "playlistfilterproxymodel.h"
#include "playlistmultiproxiesmodel.h"

class PlaylistsManager : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit PlaylistsManager(QObject *parent = nullptr);

    Q_PROPERTY(PlaylistFilterProxyModel *activePlaylist READ activePlaylist NOTIFY activePlaylistChanged FINAL)
    Q_SIGNAL void activePlaylistChanged();
    PlaylistFilterProxyModel *activePlaylist() const;

    Q_PROPERTY(PlaylistFilterProxyModel *visiblePlaylist READ visiblePlaylist NOTIFY visiblePlaylistChanged FINAL)
    Q_SIGNAL void visiblePlaylistChanged();
    PlaylistFilterProxyModel *visiblePlaylist() const;

    Q_PROPERTY(PlaylistFilterProxyModel *defaultPlaylist READ defaultPlaylist NOTIFY defaultPlaylistChanged FINAL)
    Q_SIGNAL void defaultPlaylistChanged();
    PlaylistFilterProxyModel *defaultPlaylist() const;

    Q_PROPERTY(PlaylistMultiProxiesModel *playlists READ playlists WRITE setPlaylists NOTIFY playlistsChanged)
    Q_SIGNAL void playlistsChanged();
    PlaylistMultiProxiesModel *playlists();
    void setPlaylists(PlaylistMultiProxiesModel *model);

    Q_INVOKABLE void initialize(QList<QUrl> urls = {});
    Q_INVOKABLE void playNext();
    Q_INVOKABLE void playPrevious();
    Q_INVOKABLE QString activeItemPath();

Q_SIGNALS:
    void playingItemChanged();

private:
    std::unique_ptr<PlaylistMultiProxiesModel> m_playlists = std::make_unique<PlaylistMultiProxiesModel>(nullptr);
    PlaylistItem m_playingItem;
};

#endif // PLAYLISTSMANAGER_H
