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
#include "playlistsortpropertymodel.h"
#include "playlistsortpropertyproxymodel.h"

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
        None        = Qt::DisplayRole,
        FileName    = PlaylistModel::Roles::NameRole,
        Duration    = PlaylistModel::Roles::DurationRole,
        Date        = PlaylistModel::Roles::DateRole,
        FileSize    = PlaylistModel::Roles::FileSizeRole,
        Title       = PlaylistModel::Roles::TitleRole,
        // Audio
        TrackNo     = PlaylistModel::Roles::TrackNoRole,
        SampleRate  = PlaylistModel::Roles::SampleRateRole,
        Bitrate     = PlaylistModel::Roles::BitrateRole,
        // Custom role, indicates a combination of multiple sort roles or at least one group role
        Custom,
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

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;
    void setSourceModel(QAbstractItemModel *sourceModel) override;

    void setSortPreset(Sort sortRole);
    void setSortOrder(Qt::SortOrder order);
    void sortItems();

    void addToActiveSortProperties(int sort);
    void addToActiveGroup(Group group);
    void removeFromActiveSortProperties(uint index);
    void removeFromActiveGroup(uint index);

private:
    Qt::SortOrder m_sortingOrder{Qt::SortOrder::AscendingOrder};
    Sort m_sortPreset{Sort::None};

    void onActiveSortPropertiesChanged();
    void onActiveGroupsChanged();
    void setGroupHideBlank(uint index, bool hide);
    void setSortPropertySortingOrder(uint index, int order);
    int compareVariants(const QVariant &l, const QVariant &r, SortProperty::Type type) const;
    void recreateSections();
    void scheduleSectionRecreation();
    QStringList getSectionLists(const QModelIndex &index);

    std::unique_ptr<PlaylistSortPropertyModel> m_activeSortProperties; // Current sort/group options that are actively used in the sort
    std::unique_ptr<PlaylistSortPropertyModel> m_availableSortProperties; // Unused sort/group options, benched and ready
    std::unique_ptr<PlaylistSortPropertyProxyModel> m_availableSortPropertiesProxy;
    std::unique_ptr<PlaylistSortPropertyModel> m_activeGroups; // Current group options that are actively used in the sections
    std::unique_ptr<PlaylistSortPropertyModel> m_availableGroups; // Unused group options, benched and ready
    std::unique_ptr<PlaylistSortPropertyProxyModel> m_availableGroupsProxy;
    bool m_showSections{true};
    bool m_scheduledSectionRecreation{false};

    // Qt's section property in ListView is a string that enables automatic section creation. It compares section strings of current and
    // previous item and decides whether or not show the section delegate for the current item (only shows if it is different from the previous one).
    // It does not allow arrays or other data types so in order to show multiple sections, we need to manually combine them into a string during runtime.
    // This property holds the mapping from Qt section to a list of group properties (for example: "Album · Artist" -> ["Album", "Artist"]).
    QHash<QString, QStringList> m_sectionMap;
    // Item index to Qt section map. Since some items are going to be sharing the same Qt section, using a string pointer.
    QHash<uint, QSharedPointer<QString>> m_indexToSection;
    QHash<QString, QSharedPointer<QString>> m_sectionToPtr;
};
Q_DECLARE_METATYPE(PlaylistSortProxyModel::Sort)
Q_DECLARE_METATYPE(PlaylistSortProxyModel::Group)
#endif // PLAYLISTSORTPROXYMODEL_H
