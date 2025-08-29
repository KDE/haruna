/*
 * SPDX-FileCopyrightText: 2025 Muhammet Sadık Uğursoy <sadikugursoy@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef PLAYLISTMULTIPROXIESMODEL_H
#define PLAYLISTMULTIPROXIESMODEL_H

#include "playlistfilterproxymodel.h"

class PlaylistMultiProxiesModel : public QAbstractListModel
{
    Q_OBJECT
    QML_NAMED_ELEMENT(PlaylistMultiProxiesModel)

public:
    explicit PlaylistMultiProxiesModel(QObject *parent = nullptr);
    friend class MpvItem;

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

    Q_INVOKABLE void addPlaylist(QString playlistName, QUrl internalUrl = QUrl());
    Q_INVOKABLE void removePlaylist(uint pIndex);
    Q_INVOKABLE void movePlaylist(uint row, uint destinationRow);
    Q_INVOKABLE void renamePlaylist(uint pIndex);
    Q_INVOKABLE void resetTabView();

Q_SIGNALS:
    void playingItemChanged();
    void visibleIndexChanged();
    void activeIndexChanged();

private:
    PlaylistFilterProxyModel *activeFilterProxy();
    PlaylistFilterProxyModel *visibleFilterProxy();
    PlaylistFilterProxyModel *defaultFilterProxy();
    PlaylistFilterProxyModel *getFilterProxy(QString playlistName);

    QUrl getPlaylistCacheUrl();
    QUrl getPlaylistUrl(QString playlistName);
    void saveVisiblePlaylist();
    void savePlaylistCache();

    std::vector<std::unique_ptr<PlaylistFilterProxyModel>> m_playlistFilterProxyModels;
    uint m_activeIndex{0};
    uint m_visibleIndex{0};
};

#endif // PLAYLISTMULTIPROXIESMODEL_H
