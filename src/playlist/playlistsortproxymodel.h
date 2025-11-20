/*
 * SPDX-FileCopyrightText: 2025 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef PLAYLISTSORTPROXYMODEL_H
#define PLAYLISTSORTPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QtQml/qqmlregistration.h>

#include "playlistmodel.h"

class PlaylistSortProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
    QML_NAMED_ELEMENT(PlaylistSortProxyModel)

public:
    explicit PlaylistSortProxyModel(QObject *parent = nullptr);
    friend class PlaylistFilterProxyModel;

    // Properties that are mostly unique, ideal for sorting
    // clang-format off
    enum Sort {
        None         = Qt::DisplayRole,
        FileName    = PlaylistModel::Roles::NameRole,
        Duration    = PlaylistModel::Roles::DurationRole,
        Date        = PlaylistModel::Roles::DateRole,
        FileSize    = PlaylistModel::Roles::FileSizeRole,
        Title       = PlaylistModel::Roles::TitleRole,
        // Audio
        TrackNo     = PlaylistModel::Roles::TrackNoRole,
        SampleRate  = PlaylistModel::Roles::SampleRateRole,
        Bitrate     = PlaylistModel::Roles::BitrateRole,
    };
    Q_ENUM(Sort)
    // clang-format on

    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;

    void sortItems(Sort sortRole);
    void sortItems(Qt::SortOrder order);

private:
    Qt::SortOrder m_sortingOrder{Qt::SortOrder::AscendingOrder};
    Sort m_sortRole{Sort::None};
};

#endif // PLAYLISTSORTPROXYMODEL_H
