/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef PLAYLISTFILTERPROXYMODEL_H
#define PLAYLISTFILTERPROXYMODEL_H

#include "playlistmodel.h"
#include "playlistproxymodel.h"
#include "playlistsortproxymodel.h"

class PlaylistModel;
class PlaylistSortProxyModel;
class PlaylistProxyModel;
class PlaylistFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
    QML_NAMED_ELEMENT(PlaylistFilterProxyModel)
public:
    explicit PlaylistFilterProxyModel(QObject *parent = nullptr);

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

    // PlaylistSortProxyModel
    Q_INVOKABLE void sortItems(PlaylistSortProxyModel::Sort sortMode);

    // PlaylistModel
    Q_INVOKABLE void clear();
    Q_INVOKABLE void addItem(const QString &path, PlaylistModel::Behavior behavior);
    Q_INVOKABLE void addItem(const QUrl &url, PlaylistModel::Behavior behavior);
    Q_INVOKABLE void addItems(const QList<QUrl> &urls, PlaylistModel::Behavior behavior);

private:
    // Model getters for convenience
    PlaylistProxyModel *playlistProxyModel() const;
    PlaylistSortProxyModel *sortFilterModel() const;
    PlaylistModel *playlistModel() const;

    QModelIndex mapFromPlaylistModel(uint row) const;
    QModelIndex mapToPlaylistModel(uint row) const;
};

#endif // PLAYLISTFILTERPROXYMODEL_H
