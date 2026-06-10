/*
 * SPDX-FileCopyrightText: 2025 Muhammet Sadık Uğursoy <sadikugursoy@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef PLAYLISTMULTIPROXIESMODEL_H
#define PLAYLISTMULTIPROXIESMODEL_H

#include <QJsonObject>

#include "playlistfilterproxymodel.h"

struct PlaylistsModelItem {
    QString playlistName;
    QUrl playlistUrl;
    bool isInitialized{false};
    QJsonObject jsonData;
    std::unique_ptr<PlaylistFilterProxyModel> playlist;
};

class PlaylistMultiProxiesModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit PlaylistMultiProxiesModel(QObject *parent = nullptr);

    enum Roles {
        NameRole = Qt::UserRole,
        VisibleRole,
        ActiveRole,
    };
    Q_ENUM(Roles)

    Q_PROPERTY(uint activeIndex READ activeIndex WRITE setActiveIndex NOTIFY activeIndexChanged)
    uint activeIndex();
    void setActiveIndex(uint index);

    Q_PROPERTY(uint visibleIndex READ visibleIndex WRITE setVisibleIndex NOTIFY visibleIndexChanged)
    uint visibleIndex();
    void setVisibleIndex(uint index);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void createNewPlaylist(const QString &playlistName);
    Q_INVOKABLE void removePlaylist(uint pIndex);
    Q_INVOKABLE void movePlaylist(uint row, uint destinationRow);
    Q_INVOKABLE void renamePlaylist(uint pIndex);
    Q_INVOKABLE void resetTabView();

    PlaylistFilterProxyModel *activeFilterProxy();
    PlaylistFilterProxyModel *visibleFilterProxy();
    PlaylistFilterProxyModel *defaultFilterProxy();

Q_SIGNALS:
    void playingItemChanged();
    void visibleIndexChanged();
    void activeIndexChanged();

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
};

#endif // PLAYLISTMULTIPROXIESMODEL_H
