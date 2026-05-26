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
        // set last session's active playlist as visible
        m_playlists->setVisibleIndex(m_playlists->activeIndex());

        if (!PlaybackSettings::openLastPlayedFile()) {
            return;
        }

        // restore last session's non default playlist and its playing item
        if (m_playlists->activeIndex() > 0) {
            const auto playingItem = activePlaylist()->playlistModel()->playingItem();
            activePlaylist()->setPlayingItem(playingItem);
            return;
        }

        // restore last session's default playlist
        if (!GeneralSettings::lastPlaylist().isEmpty()) {
            defaultPlaylist()->addItem(GeneralSettings::lastPlaylist(), PlaylistModel::Clear);
            return;
        }
        defaultPlaylist()->addItem(GeneralSettings::lastPlayedFile(), PlaylistModel::Clear);
        return;
    }

    const auto playlistAddMode = urls.size() == 1 ? PlaylistModel::Clear : PlaylistModel::Append;
    for (const auto &url : urls) {
        if (!url.isValid()) {
            continue;
        }
        defaultPlaylist()->addItem(url, playlistAddMode);
    }
    defaultPlaylist()->setPlayingItem(0);
}
