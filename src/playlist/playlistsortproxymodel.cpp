/*
 * SPDX-FileCopyrightText: 2025 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <KLocalizedString>

#include "playlistsortproxymodel.h"
#include "playlistmodel.h"

using Category = PlaylistGroupPropertyModel::GroupCategory;
using namespace Qt::StringLiterals;

PlaylistSortProxyModel::PlaylistSortProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , m_activeGroups{std::make_unique<PlaylistGroupPropertyModel>(this)}
    , m_availableGroups{std::make_unique<PlaylistGroupPropertyModel>(this)}
    , m_availableGroupsProxy{std::make_unique<PlaylistGroupPropertyProxyModel>(this)}
{
    setDynamicSortFilter(true);
    m_availableGroupsProxy->setSourceModel(m_availableGroups.get());

    // Set available grouping properties
    GroupProperty groupProperty;
    groupProperty.text = i18nc("@label:button folder", "Folder");
    groupProperty.group = Group::Folder;
    groupProperty.category = Category::FileCategory;
    groupProperty.type = GroupProperty::Type::StringType;
    m_availableGroups->appendGroupProperty(groupProperty);

    groupProperty.text = i18nc("@label:button file type", "Type");
    groupProperty.group = Group::FileType;
    groupProperty.category = Category::FileCategory;
    groupProperty.type = GroupProperty::Type::StringType;
    m_availableGroups->appendGroupProperty(groupProperty);

    groupProperty.text = i18nc("@label:button file extension", "Extension");
    groupProperty.group = Group::Extension;
    groupProperty.category = Category::FileCategory;
    groupProperty.type = GroupProperty::Type::StringType;
    m_availableGroups->appendGroupProperty(groupProperty);

    groupProperty.text = i18nc("@label:button disc no", "Disc No");
    groupProperty.group = Group::DiscNo;
    groupProperty.category = Category::AudioCategory;
    groupProperty.type = GroupProperty::Type::IntType;
    m_availableGroups->appendGroupProperty(groupProperty);

    groupProperty.text = i18nc("@label:button release year", "Release Year");
    groupProperty.group = Group::ReleaseYear;
    groupProperty.category = Category::AudioCategory;
    groupProperty.type = GroupProperty::Type::IntType;
    m_availableGroups->appendGroupProperty(groupProperty);

    groupProperty.text = i18nc("@label:button genre", "Genre");
    groupProperty.group = Group::Genre;
    groupProperty.category = Category::AudioCategory;
    groupProperty.type = GroupProperty::Type::StringType;
    m_availableGroups->appendGroupProperty(groupProperty);

    groupProperty.text = i18nc("@label:button album", "Album");
    groupProperty.group = Group::Album;
    groupProperty.category = Category::AudioCategory;
    groupProperty.type = GroupProperty::Type::StringType;
    m_availableGroups->appendGroupProperty(groupProperty);

    groupProperty.text = i18nc("@label:button artist", "Artist");
    groupProperty.group = Group::Artist;
    groupProperty.category = Category::AudioCategory;
    groupProperty.type = GroupProperty::Type::StringType;
    m_availableGroups->appendGroupProperty(groupProperty);

    groupProperty.text = i18nc("@label:button album artist", "Album Artist");
    groupProperty.group = Group::AlbumArtist;
    groupProperty.category = Category::AudioCategory;
    groupProperty.type = GroupProperty::Type::StringType;
    m_availableGroups->appendGroupProperty(groupProperty);

    groupProperty.text = i18nc("@label:button composer", "Composer");
    groupProperty.group = Group::Composer;
    groupProperty.category = Category::AudioCategory;
    groupProperty.type = GroupProperty::Type::StringType;
    m_availableGroups->appendGroupProperty(groupProperty);

    groupProperty.text = i18nc("@label:button lyricist", "Lyricist");
    groupProperty.group = Group::Lyricist;
    groupProperty.category = Category::AudioCategory;
    groupProperty.type = GroupProperty::Type::StringType;
    m_availableGroups->appendGroupProperty(groupProperty);

    groupProperty.text = i18nc("@label:button audio codec", "Audio Codec");
    groupProperty.group = Group::AudioCodec;
    groupProperty.category = Category::AudioCategory;
    groupProperty.type = GroupProperty::Type::StringType;
    m_availableGroups->appendGroupProperty(groupProperty);

    groupProperty.text = i18nc("@label:button framerate", "Framerate");
    groupProperty.group = Group::Framerate;
    groupProperty.category = Category::VideoCategory;
    groupProperty.type = GroupProperty::Type::RealType;
    m_availableGroups->appendGroupProperty(groupProperty);

    groupProperty.text = i18nc("@label:button video codec", "Video Codec");
    groupProperty.group = Group::VideoCodec;
    groupProperty.category = Category::VideoCategory;
    groupProperty.type = GroupProperty::Type::StringType;
    m_availableGroups->appendGroupProperty(groupProperty);

    groupProperty.text = i18nc("@label:button video orientation", "Orientation");
    groupProperty.group = Group::Orientation;
    groupProperty.category = Category::VideoCategory;
    groupProperty.type = GroupProperty::Type::IntType;
    m_availableGroups->appendGroupProperty(groupProperty);

    groupProperty.text = i18nc("@label:button resolution", "Resolution");
    groupProperty.group = Group::Resolution;
    groupProperty.category = Category::VideoCategory;
    groupProperty.type = GroupProperty::Type::StringType;
    m_availableGroups->appendGroupProperty(groupProperty);

    connect(m_activeGroups.get(), &PlaylistGroupPropertyModel::propertiesChanged, this, &PlaylistSortProxyModel::onActiveGroupChanged);
}

bool PlaylistSortProxyModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    // Compare each group role in order
    for (auto &groupProperty : std::as_const(m_activeGroups->m_properties)) {
        int compare = 0;

        // Handle special comparison cases
        if (groupProperty.group == Group::Resolution) {
            // Sort by width first, then height
            auto leftH = sourceModel()->data(source_left, PlaylistModel::DisplayedHeightRole);
            auto leftW = sourceModel()->data(source_left, PlaylistModel::DisplayedWidthRole);

            auto rightH = sourceModel()->data(source_right, PlaylistModel::DisplayedHeightRole);
            auto rightW = sourceModel()->data(source_right, PlaylistModel::DisplayedWidthRole);

            compare = compareVariants(leftH, rightH, GroupProperty::Type::IntType);
            if (compare == 0) {
                compare = compareVariants(leftW, rightW, GroupProperty::Type::IntType);
            }

        } else {
            // Common basic comparison cases
            QVariant left = sourceModel()->data(source_left, groupProperty.group);
            QVariant right = sourceModel()->data(source_right, groupProperty.group);
            compare = compareVariants(left, right, groupProperty.type);
        }

        if (compare != 0) {
            auto order = PlaylistGroupPropertyModel::GroupSortOrder(groupProperty.order);
            switch (order) {
            case PlaylistGroupPropertyModel::SameAsPrimary:
                switch (m_sortingOrder) {
                case Qt::AscendingOrder:
                    return compare < 0;
                case Qt::DescendingOrder:
                    return compare > 0;
                }
            case PlaylistGroupPropertyModel::Ascending: {
                return compare < 0;
            }
            case PlaylistGroupPropertyModel::Descending: {
                return compare > 0;
            }
            }
        }
    }

    int primarySortRole = sortRole();
    QVariant left = sourceModel()->data(source_left, primarySortRole);
    QVariant right = sourceModel()->data(source_right, primarySortRole);
    int compare;

    if (primarySortRole == Sort::FileSize) {
        int l = left.toInt();
        int r = right.toInt();
        compare = l - r;
    } else {
        compare = QString::compare(left.toString(), right.toString());
    }

    switch (m_sortingOrder) {
    case Qt::AscendingOrder:
        return compare < 0;
    case Qt::DescendingOrder:
        return compare > 0;
    }
    return source_left.row() < source_right.row(); // fallback
}

void PlaylistSortProxyModel::sortItems(Sort role)
{
    invalidate();
    m_sortRole = role;
    if (role == Sort::None) {
        // Revert any sorting done and reflect playlistmodel
        sort(-1, Qt::SortOrder::AscendingOrder);
        return;
    }
    setSortRole(m_sortRole);
    // Note that Qt::SortOrder::AscendingOrder is given here regardless of m_sortingOrder
    // because  the order will be handled inside custom lessThan() method.
    sort(0, Qt::SortOrder::AscendingOrder);
}

void PlaylistSortProxyModel::sortItems(Qt::SortOrder order)
{
    m_sortingOrder = order;
    sortItems(m_sortRole);
}

void PlaylistSortProxyModel::addToActiveGroup(Group group)
{
    if (group == Group::Separator) {
        GroupProperty groupProperty;
        groupProperty.text = i18nc("@label:button put new line", "New Line");
        groupProperty.category = Category::SeparatorCategory;
        groupProperty.group = Group::Separator;
        m_activeGroups->appendGroupProperty(groupProperty);
        return;
    }
    GroupProperty property = m_availableGroups->takeGroupProperty(group);
    m_activeGroups->appendGroupProperty(property);
}

void PlaylistSortProxyModel::removeFromActiveGroup(uint index)
{
    GroupProperty property = m_activeGroups->m_properties.at(index);
    m_activeGroups->removeProperty(index);
    if (property.group == Group::Separator) {
        return;
    }
    m_availableGroups->appendGroupProperty(property);
}

void PlaylistSortProxyModel::onActiveGroupChanged()
{
    sortItems(m_sortRole);
}

void PlaylistSortProxyModel::setGroupDisplay(uint index, int display)
{
    m_activeGroups->m_properties[index].display = PlaylistGroupPropertyModel::GroupDisplay(display);
    QModelIndex activeGroupIndex = m_activeGroups->index(index, 0);
    Q_EMIT m_activeGroups->dataChanged(activeGroupIndex, activeGroupIndex);
    onActiveGroupChanged();
}

void PlaylistSortProxyModel::setGroupSortOrder(uint index, int order)
{
    m_activeGroups->m_properties[index].order = PlaylistGroupPropertyModel::GroupSortOrder(order);
    QModelIndex activeGroupIndex = m_activeGroups->index(index, 0);
    Q_EMIT m_activeGroups->dataChanged(activeGroupIndex, activeGroupIndex);
    onActiveGroupChanged();
}

int PlaylistSortProxyModel::compareVariants(const QVariant &l, const QVariant &r, GroupProperty::Type type) const
{
    switch (type) {
    case GroupProperty::StringType: {
        return QString::compare(l.toString(), r.toString(), Qt::CaseInsensitive);
    }
    case GroupProperty::IntType: {
        int a = l.toInt();
        int b = r.toInt();
        return (a < b) ? -1 : ((a > b) ? 1 : 0);
    }
    case GroupProperty::RealType: {
        double a = l.toDouble();
        double b = r.toDouble();
        return (a < b) ? -1 : ((a > b) ? 1 : 0);
    }
    default: {
        return 0;
    }
    }
}

#include "moc_playlistsortproxymodel.cpp"
