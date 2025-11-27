/*
 * SPDX-FileCopyrightText: 2025 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef PLAYLISTSORTPROXYMODEL_H
#define PLAYLISTSORTPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QtQml/qqmlregistration.h>

#include "playlistgrouppropertymodel.h"
#include "playlistgrouppropertyproxymodel.h"
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

    // Properties that are mostly common between items, ideal for grouping
    enum Group {
        Folder      = PlaylistModel::Roles::DirNameRole,
        FileType    = PlaylistModel::Roles::TypeRole,
        Extension   = PlaylistModel::Roles::ExtensionRole,
        // Audio
        DiscNo      = PlaylistModel::Roles::DiscNoRole,
        ReleaseYear = PlaylistModel::Roles::ReleaseYearRole,
        Genre       = PlaylistModel::Roles::GenreRole,
        Album       = PlaylistModel::Roles::AlbumRole,
        Artist      = PlaylistModel::Roles::ArtistRole,
        AlbumArtist = PlaylistModel::Roles::AlbumArtistRole,
        Composer    = PlaylistModel::Roles::ComposerRole,
        Lyricist    = PlaylistModel::Roles::LyricistRole,
        AudioCodec  = PlaylistModel::Roles::AudioCodecRole,
        // Video
        Framerate   = PlaylistModel::Roles::FramerateRole,
        VideoCodec  = PlaylistModel::Roles::VideoCodecRole,
        Orientation = PlaylistModel::Roles::OrientationRole,
        // Group-unique roles, that requires more than one PlaylistModel roles. Handled separately.
        Resolution,
        Separator,
    };
    Q_ENUM(Group)
    // clang-format on

    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;

    void sortItems(Sort sortRole);
    void sortItems(Qt::SortOrder order);

    void addToActiveGroup(Group group);
    void removeFromActiveGroup(uint index);

private:
    Qt::SortOrder m_sortingOrder{Qt::SortOrder::AscendingOrder};
    Sort m_sortRole{Sort::None};

    void onActiveGroupChanged();
    void setGroupDisplay(uint index, int display);
    void setGroupSortOrder(uint index, int order);

    int compareVariants(const QVariant &l, const QVariant &r, GroupProperty::Type type) const;

    std::unique_ptr<PlaylistGroupPropertyModel> m_activeGroups; // Current group options that are actively used in the sort
    std::unique_ptr<PlaylistGroupPropertyModel> m_availableGroups; // Unused group options, benched and ready
    std::unique_ptr<PlaylistGroupPropertyProxyModel> m_availableGroupsProxy;
};
Q_DECLARE_METATYPE(PlaylistSortProxyModel::Sort)
Q_DECLARE_METATYPE(PlaylistSortProxyModel::Group)
#endif // PLAYLISTSORTPROXYMODEL_H
