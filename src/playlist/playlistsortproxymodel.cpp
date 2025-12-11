/*
 * SPDX-FileCopyrightText: 2025 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QTimer>

#include <KLocalizedString>

#include "playlistsortproxymodel.h"
#include "playlistmodel.h"

using Category = PlaylistSortPropertyModel::Category;
using namespace Qt::StringLiterals;

PlaylistSortProxyModel::PlaylistSortProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , m_activeSortProperties{std::make_unique<PlaylistSortPropertyModel>(this)}
    , m_availableSortProperties{std::make_unique<PlaylistSortPropertyModel>(this)}
    , m_availableSortPropertiesProxy{std::make_unique<PlaylistSortPropertyProxyModel>(this)}
    , m_activeGroups{std::make_unique<PlaylistSortPropertyModel>(this)}
    , m_availableGroups{std::make_unique<PlaylistSortPropertyModel>(this)}
    , m_availableGroupsProxy{std::make_unique<PlaylistSortPropertyProxyModel>(this)}
{
    setDynamicSortFilter(true);
    m_availableSortPropertiesProxy->setSourceModel(m_availableSortProperties.get());
    m_availableGroupsProxy->setSourceModel(m_availableGroups.get());

    SortProperty sortProperty;
    // Set available sorting properties
    sortProperty.text = i18nc("@label:button file name", "File Name");
    sortProperty.sort = Sort::FileName;
    sortProperty.category = Category::FileCategory;
    sortProperty.type = SortProperty::Type::StringType;
    m_availableSortProperties->appendSortProperty(sortProperty);

    sortProperty.text = i18nc("@label:button duration", "Duration");
    sortProperty.sort = Sort::Duration;
    sortProperty.category = Category::FileCategory;
    sortProperty.type = SortProperty::Type::StringType; // formattedDuration is a QString
    m_availableSortProperties->appendSortProperty(sortProperty);

    sortProperty.text = i18nc("@label:button date", "Date");
    sortProperty.sort = Sort::Date;
    sortProperty.category = Category::FileCategory;
    sortProperty.type = SortProperty::Type::DateType;
    m_availableSortProperties->appendSortProperty(sortProperty);

    sortProperty.text = i18nc("@label:button file size", "File Size");
    sortProperty.sort = Sort::FileSize;
    sortProperty.category = Category::FileCategory;
    sortProperty.type = SortProperty::Type::IntType;
    m_availableSortProperties->appendSortProperty(sortProperty);

    sortProperty.text = i18nc("@label:button title", "Title");
    sortProperty.sort = Sort::Title;
    sortProperty.category = Category::FileCategory;
    sortProperty.type = SortProperty::Type::StringType;
    m_availableSortProperties->appendSortProperty(sortProperty);

    sortProperty.text = i18nc("@label:button track no", "Track No");
    sortProperty.sort = Sort::TrackNo;
    sortProperty.category = Category::AudioCategory;
    sortProperty.type = SortProperty::Type::IntType;
    m_availableSortProperties->appendSortProperty(sortProperty);

    sortProperty.text = i18nc("@label:button sample rate", "Sample Rate");
    sortProperty.sort = Sort::SampleRate;
    sortProperty.category = Category::AudioCategory;
    sortProperty.type = SortProperty::Type::IntType;
    m_availableSortProperties->appendSortProperty(sortProperty);

    sortProperty.text = i18nc("@label:button bitrate", "Bitrate");
    sortProperty.sort = Sort::Bitrate;
    sortProperty.category = Category::AudioCategory;
    sortProperty.type = SortProperty::Type::IntType;
    m_availableSortProperties->appendSortProperty(sortProperty);

    // Set available grouping properties
    sortProperty.text = i18nc("@label:button folder", "Folder");
    sortProperty.sort = Group::Folder;
    sortProperty.category = Category::FileCategory;
    sortProperty.type = SortProperty::Type::StringType;
    m_availableSortProperties->appendSortProperty(sortProperty);
    m_availableGroups->appendSortProperty(sortProperty);

    sortProperty.text = i18nc("@label:button file type", "Type");
    sortProperty.sort = Group::FileType;
    sortProperty.category = Category::FileCategory;
    sortProperty.type = SortProperty::Type::StringType;
    m_availableSortProperties->appendSortProperty(sortProperty);
    m_availableGroups->appendSortProperty(sortProperty);

    sortProperty.text = i18nc("@label:button file extension", "Extension");
    sortProperty.sort = Group::Extension;
    sortProperty.category = Category::FileCategory;
    sortProperty.type = SortProperty::Type::StringType;
    m_availableSortProperties->appendSortProperty(sortProperty);
    m_availableGroups->appendSortProperty(sortProperty);

    sortProperty.text = i18nc("@label:button disc no", "Disc No");
    sortProperty.sort = Group::DiscNo;
    sortProperty.category = Category::AudioCategory;
    sortProperty.type = SortProperty::Type::IntType;
    m_availableSortProperties->appendSortProperty(sortProperty);
    m_availableGroups->appendSortProperty(sortProperty);

    sortProperty.text = i18nc("@label:button release year", "Release Year");
    sortProperty.sort = Group::ReleaseYear;
    sortProperty.category = Category::AudioCategory;
    sortProperty.type = SortProperty::Type::IntType;
    m_availableSortProperties->appendSortProperty(sortProperty);
    m_availableGroups->appendSortProperty(sortProperty);

    sortProperty.text = i18nc("@label:button genre", "Genre");
    sortProperty.sort = Group::Genre;
    sortProperty.category = Category::AudioCategory;
    sortProperty.type = SortProperty::Type::StringType;
    m_availableSortProperties->appendSortProperty(sortProperty);
    m_availableGroups->appendSortProperty(sortProperty);

    sortProperty.text = i18nc("@label:button album", "Album");
    sortProperty.sort = Group::Album;
    sortProperty.category = Category::AudioCategory;
    sortProperty.type = SortProperty::Type::StringType;
    m_availableSortProperties->appendSortProperty(sortProperty);
    m_availableGroups->appendSortProperty(sortProperty);

    sortProperty.text = i18nc("@label:button artist", "Artist");
    sortProperty.sort = Group::Artist;
    sortProperty.category = Category::AudioCategory;
    sortProperty.type = SortProperty::Type::StringType;
    m_availableSortProperties->appendSortProperty(sortProperty);
    m_availableGroups->appendSortProperty(sortProperty);

    sortProperty.text = i18nc("@label:button album artist", "Album Artist");
    sortProperty.sort = Group::AlbumArtist;
    sortProperty.category = Category::AudioCategory;
    sortProperty.type = SortProperty::Type::StringType;
    m_availableSortProperties->appendSortProperty(sortProperty);
    m_availableGroups->appendSortProperty(sortProperty);

    sortProperty.text = i18nc("@label:button composer", "Composer");
    sortProperty.sort = Group::Composer;
    sortProperty.category = Category::AudioCategory;
    sortProperty.type = SortProperty::Type::StringType;
    m_availableSortProperties->appendSortProperty(sortProperty);
    m_availableGroups->appendSortProperty(sortProperty);

    sortProperty.text = i18nc("@label:button lyricist", "Lyricist");
    sortProperty.sort = Group::Lyricist;
    sortProperty.category = Category::AudioCategory;
    sortProperty.type = SortProperty::Type::StringType;
    m_availableSortProperties->appendSortProperty(sortProperty);
    m_availableGroups->appendSortProperty(sortProperty);

    sortProperty.text = i18nc("@label:button audio codec", "Audio Codec");
    sortProperty.sort = Group::AudioCodec;
    sortProperty.category = Category::AudioCategory;
    sortProperty.type = SortProperty::Type::StringType;
    m_availableSortProperties->appendSortProperty(sortProperty);
    m_availableGroups->appendSortProperty(sortProperty);

    sortProperty.text = i18nc("@label:button framerate", "Framerate");
    sortProperty.sort = Group::Framerate;
    sortProperty.category = Category::VideoCategory;
    sortProperty.type = SortProperty::Type::RealType;
    m_availableSortProperties->appendSortProperty(sortProperty);
    m_availableGroups->appendSortProperty(sortProperty);

    sortProperty.text = i18nc("@label:button video codec", "Video Codec");
    sortProperty.sort = Group::VideoCodec;
    sortProperty.category = Category::VideoCategory;
    sortProperty.type = SortProperty::Type::StringType;
    m_availableSortProperties->appendSortProperty(sortProperty);
    m_availableGroups->appendSortProperty(sortProperty);

    sortProperty.text = i18nc("@label:button video orientation", "Orientation");
    sortProperty.sort = Group::Orientation;
    sortProperty.category = Category::VideoCategory;
    sortProperty.type = SortProperty::Type::IntType;
    m_availableSortProperties->appendSortProperty(sortProperty);
    m_availableGroups->appendSortProperty(sortProperty);

    sortProperty.text = i18nc("@label:button resolution", "Resolution");
    sortProperty.sort = Group::Resolution;
    sortProperty.category = Category::VideoCategory;
    sortProperty.type = SortProperty::Type::StringType;
    m_availableSortProperties->appendSortProperty(sortProperty);
    m_availableGroups->appendSortProperty(sortProperty);

    connect(m_activeGroups.get(), &PlaylistSortPropertyModel::propertiesChanged, this, &PlaylistSortProxyModel::onActiveGroupsChanged);
    connect(m_activeSortProperties.get(), &PlaylistSortPropertyModel::propertiesChanged, this, &PlaylistSortProxyModel::onActiveSortPropertiesChanged);
}

QVariant PlaylistSortProxyModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    // Only handle Section role here
    if (role == PlaylistModel::Roles::SectionRole) {
        if (m_indexToSection.empty()) {
            return QVariant(u""_s);
        }

        QSharedPointer sectionPtr = m_indexToSection.value(index.row());
        if (!m_showSections || sectionPtr.isNull()) {
            return QVariant(u""_s);
        }
        return QVariant(*sectionPtr.data());
    }

    return sourceModel()->data(mapToSource(index), role);
}

bool PlaylistSortProxyModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    // Compare each group role in order
    for (auto &sortProperty : std::as_const(m_activeSortProperties->m_properties)) {
        int compare = 0;

        // Handle special comparison cases
        if (sortProperty.sort == Group::Resolution) {
            // Sort by width first, then height
            auto leftH = sourceModel()->data(source_left, PlaylistModel::DisplayedHeightRole);
            auto leftW = sourceModel()->data(source_left, PlaylistModel::DisplayedWidthRole);

            auto rightH = sourceModel()->data(source_right, PlaylistModel::DisplayedHeightRole);
            auto rightW = sourceModel()->data(source_right, PlaylistModel::DisplayedWidthRole);

            compare = compareVariants(leftH, rightH, SortProperty::Type::IntType);
            if (compare == 0) {
                compare = compareVariants(leftW, rightW, SortProperty::Type::IntType);
            }

        } else {
            // Common basic comparison cases
            QVariant left = sourceModel()->data(source_left, sortProperty.sort);
            QVariant right = sourceModel()->data(source_right, sortProperty.sort);
            compare = compareVariants(left, right, sortProperty.type);
        }

        if (compare != 0) {
            auto order = PlaylistSortPropertyModel::SortOrder(sortProperty.order);
            switch (order) {
            case PlaylistSortPropertyModel::SameAsPrimary:
                switch (m_sortingOrder) {
                case Qt::AscendingOrder:
                    return compare < 0;
                case Qt::DescendingOrder:
                    return compare > 0;
                }
            case PlaylistSortPropertyModel::Ascending: {
                return compare < 0;
            }
            case PlaylistSortPropertyModel::Descending: {
                return compare > 0;
            }
            }
        }
    }

    return source_left.row() < source_right.row(); // fallback
}

void PlaylistSortProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    QSortFilterProxyModel::setSourceModel(sourceModel);
    auto playlistModel = qobject_cast<PlaylistModel *>(sourceModel);

    // When a new item is inserted, recreate sections with the new metadata after it is ready
    connect(playlistModel, &PlaylistModel::metaDataReady, this, &PlaylistSortProxyModel::scheduleSectionRecreation);
    connect(this, &QAbstractItemModel::rowsInserted, this, &PlaylistSortProxyModel::scheduleSectionRecreation);
    connect(this, &QAbstractItemModel::rowsRemoved, this, &PlaylistSortProxyModel::scheduleSectionRecreation);
}

void PlaylistSortProxyModel::setSortPreset(Sort preset)
{
    m_sortPreset = preset;
    qsizetype size = m_activeSortProperties->m_properties.size();
    for (auto i = 0; i < size; ++i) {
        removeFromActiveSortProperties(0);
    }

    if (preset == Sort::None) {
        sort(-1, Qt::SortOrder::AscendingOrder);
        return;
    } else {
        addToActiveSortProperties(preset);
    }
    sortItems();
}

void PlaylistSortProxyModel::setSortOrder(Qt::SortOrder order)
{
    m_sortingOrder = order;
    if (m_sortPreset == Sort::None) {
        return;
    } else {
        sortItems();
    }
}

void PlaylistSortProxyModel::sortItems()
{
    invalidate();
    // Note that Qt::SortOrder::AscendingOrder is given here regardless of m_sortingOrder
    // because  the order will be handled inside custom lessThan() method.
    sort(0, Qt::SortOrder::AscendingOrder);
}

void PlaylistSortProxyModel::addToActiveSortProperties(int sort)
{
    SortProperty property = m_availableSortProperties->takeSortProperty(sort);
    m_activeSortProperties->appendSortProperty(property);
}

void PlaylistSortProxyModel::addToActiveGroup(Group group)
{
    if (group == Group::Separator) {
        SortProperty sortProperty;
        sortProperty.text = i18nc("@label:button put new line", "New Line");
        sortProperty.category = Category::SeparatorCategory;
        sortProperty.sort = Group::Separator;
        m_activeGroups->appendSortProperty(sortProperty);
        return;
    }
    SortProperty property = m_availableGroups->takeSortProperty(group);
    m_activeGroups->appendSortProperty(property);
}

void PlaylistSortProxyModel::removeFromActiveSortProperties(uint index)
{
    SortProperty property = m_activeSortProperties->m_properties.at(index);
    m_activeSortProperties->removeProperty(index);
    m_availableSortProperties->appendSortProperty(property);
}

void PlaylistSortProxyModel::removeFromActiveGroup(uint index)
{
    SortProperty property = m_activeGroups->m_properties.at(index);
    m_activeGroups->removeProperty(index);
    if (property.sort == Group::Separator) {
        return;
    }
    m_availableGroups->appendSortProperty(property);
}

void PlaylistSortProxyModel::onActiveSortPropertiesChanged()
{
    if (m_activeSortProperties->m_properties.isEmpty()) {
        m_sortPreset = Sort::None;
        sort(-1, Qt::SortOrder::AscendingOrder);
        return;
    }
    // Check if the change is a preset
    if (m_activeSortProperties->m_properties.size() == 1) {
        int preset = m_activeSortProperties->m_properties[0].sort;

        switch (preset) {
        case Sort::FileName:
        case Sort::Duration:
        case Sort::Date:
        case Sort::FileSize:
        case Sort::Title:
        case Sort::TrackNo:
        case Sort::SampleRate:
        case Sort::Bitrate: {
            m_sortPreset = Sort(preset);
            break;
        }
        default:
            m_sortPreset = Sort::Custom;
            break;
        }
    } else {
        m_sortPreset = Sort::Custom;
    }

    sortItems();
    recreateSections();
}

void PlaylistSortProxyModel::onActiveGroupsChanged()
{
    recreateSections();
}

void PlaylistSortProxyModel::setGroupHideBlank(uint index, bool hide)
{
    m_activeGroups->m_properties[index].hideBlank = hide;
    QModelIndex activeGroupIndex = m_activeGroups->index(index, 0);
    Q_EMIT m_activeGroups->dataChanged(activeGroupIndex, activeGroupIndex);
    onActiveGroupsChanged();
}

void PlaylistSortProxyModel::setSortPropertySortingOrder(uint index, int order)
{
    m_activeSortProperties->m_properties[index].order = PlaylistSortPropertyModel::SortOrder(order);
    QModelIndex activeSortPropertyIndex = m_activeSortProperties->index(index, 0);
    Q_EMIT m_activeSortProperties->dataChanged(activeSortPropertyIndex, activeSortPropertyIndex);
    onActiveSortPropertiesChanged();
}

int PlaylistSortProxyModel::compareVariants(const QVariant &l, const QVariant &r, SortProperty::Type type) const
{
    switch (type) {
    case SortProperty::StringType: {
        return QString::compare(l.toString(), r.toString(), Qt::CaseInsensitive);
    }
    case SortProperty::IntType: {
        int a = l.toInt();
        int b = r.toInt();
        return (a < b) ? -1 : ((a > b) ? 1 : 0);
    }
    case SortProperty::RealType: {
        double a = l.toDouble();
        double b = r.toDouble();
        return (a < b) ? -1 : ((a > b) ? 1 : 0);
    }
    case SortProperty::DateType: {
        QDateTime a = l.toDateTime();
        QDateTime b = r.toDateTime();
        return (a < b) ? -1 : ((a > b) ? 1 : 0);
    }
    default: {
        return 0;
    }
    }
}

void PlaylistSortProxyModel::recreateSections()
{
    // After sorting is done, re-evaluate the sections and store them
    m_sectionMap.clear();
    m_sectionToPtr.clear();
    m_indexToSection.clear();

    if (!m_showSections || m_activeGroups->m_properties.isEmpty()) {
        Q_EMIT dataChanged(index(0, 0), index(rowCount() - 1, 0));
        return;
    }

    for (int i = 0; i < rowCount(); ++i) {
        QStringList sectionList = getSectionLists(index(i, 0));
        QString section = sectionList.join(u" · "); // Join with Centered Dot

        if (!m_sectionMap.contains(section)) {
            m_sectionMap.insert(section, sectionList);
            auto ptr = QSharedPointer<QString>::create(section);
            m_sectionToPtr.insert(section, ptr);
            m_indexToSection.insert(i, ptr);
        } else {
            // Same section string, find the instance of it an point to it
            m_indexToSection.insert(i, m_sectionToPtr.value(section));
        }
    }
    Q_EMIT dataChanged(index(0, 0), index(rowCount() - 1, 0));
}

void PlaylistSortProxyModel::scheduleSectionRecreation()
{
    if (m_scheduledSectionRecreation) {
        return;
    }
    m_scheduledSectionRecreation = true;

    // Allow queuing the recreate function to be executed once at the end of the event loop
    QTimer::singleShot(0, this, [this]() {
        recreateSections();
        m_scheduledSectionRecreation = false;
    });
}

QStringList PlaylistSortProxyModel::getSectionLists(const QModelIndex &index)
{
    // Sections are calculated per item. There can be 1 or more rows in the section depending on number of NewLines.
    // 1. Each group property in the active group is extracted from the playlist items and appended to the currentSectionRow.
    // 2. If there are NewLine(s) in the active group, then currentSectionRow is joined with "·" after each NewLine.
    //    Joined string is appended into sectionRows.
    // 3. At the end, the last currentSectionRow is joined into a string by the same method and inserted into sectionRows
    // 4. sectionRows is returned

    QStringList sectionRows;
    QStringList currentSectionRow;
    QList<bool> hideBlankRow;

    // Initialize placeholder values for missing properties.
    // clang-format off
    QHash<Group, QString> placeholders = {
        {Group::Genre,         i18nc("@title:group", "No Genre")},
        {Group::Album,         i18nc("@title:group", "No Album")},
        {Group::Artist,        i18nc("@title:group", "No Artist")},
        {Group::AlbumArtist,   i18nc("@title:group", "No Album Artist")},
        {Group::Composer,      i18nc("@title:group", "No Composer")},
        {Group::Lyricist,      i18nc("@title:group", "No Lyricist")},
        {Group::ReleaseYear,   i18nc("@title:group", "No Release Year")},
        {Group::Resolution,    i18nc("@title:group", "Not a Video")},
        {Group::AudioCodec,    i18nc("@title:group", "No Audio Codec")},
        {Group::VideoCodec,    i18nc("@title:group", "No Video Codec")},
    };
    // clang-format on

    for (auto &sortProperty : std::as_const(m_activeGroups->m_properties)) {
        bool hideBlank = sortProperty.hideBlank; // show in the section if and only if it exists

        switch (Group(sortProperty.sort)) {
        case Folder: {
            QString folderPath = sourceModel()->data(mapToSource(index), sortProperty.sort).toString();
            currentSectionRow.append(folderPath);
            hideBlankRow.append(hideBlank);
            break;
        }
        case Extension: {
            QString extension = sourceModel()->data(mapToSource(index), sortProperty.sort).toString();
            currentSectionRow.append(extension);
            hideBlankRow.append(hideBlank);
            break;
        }
        case DiscNo: {
            int discNo = sourceModel()->data(mapToSource(index), sortProperty.sort).toInt();
            QString disc = i18nc("@title:group disc", "Disc") + u" " + QString::number(discNo);
            // Disc No should be 0 if the metadata field for it does not exist
            if (discNo <= 0 && hideBlank) {
                break;
            }
            currentSectionRow.append(disc);
            hideBlankRow.append(hideBlank);
            break;
        }
        case ReleaseYear: {
            int releaseYear = sourceModel()->data(mapToSource(index), sortProperty.sort).toInt();
            if (releaseYear <= 0) {
                if (!hideBlank) {
                    QString placeholder = placeholders[Group(sortProperty.sort)];
                    currentSectionRow.append(placeholder);
                    hideBlankRow.append(hideBlank);
                }
                break;
            }
            QString year = QString::number(releaseYear);
            currentSectionRow.append(year);
            hideBlankRow.append(hideBlank);
            break;
        }
        case Framerate: {
            double fps = sourceModel()->data(mapToSource(index), sortProperty.sort).toDouble();
            int fpsDecimal = int(fps);
            if (fps > fpsDecimal) {
                // Values like 29.97 or 59.94. Show them as float
                currentSectionRow.append(QString::number(fps, 'f', 3) + u"fps");
            } else {
                currentSectionRow.append(QString::number(fpsDecimal) + u"fps");
            }
            hideBlankRow.append(hideBlank);
            break;
        }
        case Resolution: {
            int height = sourceModel()->data(mapToSource(index), PlaylistModel::DisplayedHeightRole).toInt();
            int width = sourceModel()->data(mapToSource(index), PlaylistModel::DisplayedWidthRole).toInt();
            if (height == 0 || width == 0) {
                if (!hideBlank) {
                    QString placeholder = placeholders[Group(sortProperty.sort)];
                    currentSectionRow.append(placeholder);
                    hideBlankRow.append(hideBlank);
                }
                break;
            }
            QString resolution(QString::number(width) + u"×" + QString::number(height));
            currentSectionRow.append(resolution);
            hideBlankRow.append(hideBlank);
            break;
        }
        case FileType: {
            QString type = sourceModel()->data(mapToSource(index), sortProperty.sort).toString();
            if (type == u"video") {
                currentSectionRow.append(i18nc("@title:group", "Video File"));
                hideBlankRow.append(hideBlank);
            } else if (type == u"audio") {
                currentSectionRow.append(i18nc("@title:group", "Audio File"));
                hideBlankRow.append(hideBlank);
            }
            break;
        }
        case AudioCodec:
        case VideoCodec: {
            QString codec = sourceModel()->data(mapToSource(index), sortProperty.sort).toString();
            if (codec.isEmpty()) {
                if (!hideBlank) {
                    QString placeholder = placeholders[Group(sortProperty.sort)];
                    currentSectionRow.append(placeholder);
                    hideBlankRow.append(hideBlank);
                }
                break;
            }
            currentSectionRow.append(codec);
            hideBlankRow.append(hideBlank);
            break;
        }
        case Orientation: {
            int orientation = sourceModel()->data(mapToSource(index), sortProperty.sort).toInt();
            switch (VideoMetaData::Orientation(orientation)) {
            case VideoMetaData::Landscape:
                currentSectionRow.append(i18nc("@title:group", "Landscape"));
                hideBlankRow.append(hideBlank);
                break;
            case VideoMetaData::Portrait:
                currentSectionRow.append(i18nc("@title:group", "Portrait"));
                hideBlankRow.append(hideBlank);
                break;
            case VideoMetaData::Square:
                currentSectionRow.append(i18nc("@title:group", "Square"));
                hideBlankRow.append(hideBlank);
                break;
            }
            break;
        }
        // Media Data
        case Genre:
        case Album:
        case Artist:
        case AlbumArtist:
        case Composer:
        case Lyricist: {
            QString itemString = sourceModel()->data(mapToSource(index), Group(sortProperty.sort)).toString();
            if (itemString.isEmpty()) {
                if (!hideBlank) {
                    QString placeholder = placeholders[Group(sortProperty.sort)];
                    currentSectionRow.append(placeholder);
                    hideBlankRow.append(hideBlank);
                }
                break;
            }
            if (currentSectionRow.contains(itemString)) {
                // Combine the fields with the same string name if partial is set
                // ie. Album and AlbumArtist might be same for some audio files. Do not repeat them.
                if (hideBlank) {
                    break;
                }
                auto itemIndex = currentSectionRow.indexOf(itemString);
                if (hideBlankRow[itemIndex]) {
                    break;
                }
            }
            if (hideBlank && currentSectionRow.contains(itemString)) {
                break;
            }
            currentSectionRow.append(itemString);
            hideBlankRow.append(hideBlank);
            break;
        }
        case Separator: {
            if (!currentSectionRow.isEmpty()) {
                // Append U+200B (zero width space) to make the section different
                // otherwise, sections will not be updated for some cases
                currentSectionRow.back().append(u"\u200B"_s);
                sectionRows.append(currentSectionRow.join(u" · "));
                currentSectionRow.clear();
                hideBlankRow.clear();
            }
            break;
        }
        }
    }

    if (!currentSectionRow.isEmpty()) {
        sectionRows.append(currentSectionRow.join(u" · "));
    }

    return sectionRows;
}

#include "moc_playlistsortproxymodel.cpp"
