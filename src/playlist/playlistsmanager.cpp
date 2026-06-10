/*
 * SPDX-FileCopyrightText: 2026 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "playlistsmanager.h"

#include <generalsettings.h>
#include <playbacksettings.h>

PlaylistsManager::PlaylistsManager(QObject *parent)
    : QObject{parent}
{
    // clang-format off
    connect(m_playlists.get(), &PlaylistMultiProxiesModel::playingItemChanged,
            this, &PlaylistsManager::playingItemChanged);

    connect(m_playlists.get(), &PlaylistMultiProxiesModel::visibleIndexChanged,
            this, &PlaylistsManager::visiblePlaylistChanged);

    connect(m_playlists.get(), &PlaylistMultiProxiesModel::activeIndexChanged,
            this, &PlaylistsManager::activePlaylistChanged);
    // clang-format on
}

PlaylistFilterProxyModel *PlaylistsManager::activePlaylist() const
{
    return m_playlists->activeFilterProxy();
}

PlaylistFilterProxyModel *PlaylistsManager::visiblePlaylist() const
{
    return m_playlists->visibleFilterProxy();
}

PlaylistFilterProxyModel *PlaylistsManager::defaultPlaylist() const
{
    return m_playlists->defaultFilterProxy();
}

PlaylistMultiProxiesModel *PlaylistsManager::playlists()
{
    return m_playlists.get();
}

void PlaylistsManager::setPlaylists(PlaylistMultiProxiesModel *model)
{
    m_playlists.reset(model);
}

void PlaylistsManager::initialize(const QList<QUrl> urls)
{
    if (urls.isEmpty()) {
        if (!PlaybackSettings::openLastPlayedFile()) {
            return;
        }

        // restore last session's non default playlist and its playing item
        const auto playingItem = activePlaylist()->playlistModel()->playingItem();
        activePlaylist()->setPlayingItem(playingItem);
        return;
    }

    if (urls.count() == 1) {
        defaultPlaylist()->addItem(urls.first(), PlaylistModel::Clear);
    }

    if (urls.count() > 1) {
        for (const auto &url : urls) {
            if (!url.isValid()) {
                continue;
            }
            defaultPlaylist()->addItem(url, PlaylistModel::Append);
        }
        defaultPlaylist()->setPlayingItem(0);
    }
}

void PlaylistsManager::playNext()
{
    activePlaylist()->playNext();
}

void PlaylistsManager::playPrevious()
{
    activePlaylist()->playPrevious();
}

QString PlaylistsManager::activeItemPath()
{
    return activePlaylist()->getFilePath(activePlaylist()->getPlayingItem());
}
