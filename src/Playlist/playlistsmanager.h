/*
 * SPDX-FileCopyrightText: 2025 Muhammet Sadık Uğursoy <sadikugursoy@gmail.com>
 * SPDX-FileCopyrightText: 2026 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef PLAYLISTSMANAGER_H
#define PLAYLISTSMANAGER_H

#include <QJsonObject>
#include <QTimer>

#include "playlistfilterproxymodel.h"

struct PlaylistsModelItem {
    QString playlistName;
    QUrl playlistUrl;
    bool isInitialized{false};
    QJsonObject jsonData;
    std::unique_ptr<PlaylistFilterProxyModel> playlist;
};

class PlaylistsManager : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit PlaylistsManager(QObject *parent = nullptr);

    enum Roles {
        NameRole = Qt::UserRole,
        VisibleRole,
        ActiveRole,
    };
    Q_ENUM(Roles)

    Q_PROPERTY(PlaylistFilterProxyModel *activePlaylist READ activePlaylist NOTIFY activePlaylistChanged FINAL)
    Q_SIGNAL void activePlaylistChanged();
    PlaylistFilterProxyModel *activePlaylist();
    void setActiveIndex(uint index);

    Q_PROPERTY(PlaylistFilterProxyModel *visiblePlaylist READ visiblePlaylist NOTIFY visiblePlaylistChanged FINAL)
    Q_SIGNAL void visiblePlaylistChanged();
    PlaylistFilterProxyModel *visiblePlaylist();

    Q_PROPERTY(uint visibleIndex READ visibleIndex WRITE setVisibleIndex NOTIFY visibleIndexChanged)
    Q_SIGNAL void visibleIndexChanged();
    uint visibleIndex();
    void setVisibleIndex(uint index);

    Q_PROPERTY(PlaylistFilterProxyModel *defaultPlaylist READ defaultPlaylist NOTIFY defaultPlaylistChanged FINAL)
    Q_SIGNAL void defaultPlaylistChanged();
    PlaylistFilterProxyModel *defaultPlaylist();

    Q_INVOKABLE void initialize(QList<QUrl> urls = {});
    Q_INVOKABLE void playNext();
    Q_INVOKABLE void playPrevious();
    Q_INVOKABLE QString activeItemPath();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void createNewPlaylist(const QString &playlistName);
    Q_INVOKABLE void removePlaylist(uint pIndex);
    Q_INVOKABLE void movePlaylist(uint row, uint destinationRow);
    Q_INVOKABLE void renamePlaylist(uint pIndex);
    Q_INVOKABLE void resetTabView();

Q_SIGNALS:
    void playingItemChanged();

private:
    PlaylistFilterProxyModel *getFilterProxy(const QString &playlistName);

    void addPlaylist(PlaylistsModelItem newItem);
    void initPlaylist(uint row, bool addItemsToPlaylist = true);
    QUrl getPlaylistCacheUrl();
    QUrl getDefaultPlaylistUrl();
    QUrl getPlaylistUrl(const QString &playlistName);
    void saveVisiblePlaylist();
    void savePlaylist(const QString &playlistName, PlaylistFilterProxyModel *proxyModel);
    void savePlaylistCache();

    std::vector<PlaylistsModelItem> m_data;
    uint m_activeIndex{0};
    uint m_visibleIndex{0};
    QTimer m_savePlaylistTimer;
    PlaylistItem m_playingItem;
};

#endif // PLAYLISTSMANAGER_H
