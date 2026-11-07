/*
 * SPDX-FileCopyrightText: 2025 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QTimer>

#include <KLocalizedString>

#include "playlistmodel.h"
#include "playlistsortproxymodel.h"

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
    sortProperty.sort = static_cast<int>(SortAndGroupFields::FileName);
    sortProperty.category = Category::FileCategory;
    sortProperty.type = SortProperty::Type::StringType;
    m_availableSortProperties->appendSortProperty(sortProperty);

    sortProperty.text = i18nc("@label:button duration", "Duration");
    sortProperty.sort = static_cast<int>(SortAndGroupFields::Duration);
    sortProperty.category = Category::FileCategory;
    sortProperty.type = SortProperty::Type::StringType; // formattedDuration is a QString
    m_availableSortProperties->appendSortProperty(sortProperty);

    sortProperty.text = i18nc("@label:button date", "Date");
    sortProperty.sort = static_cast<int>(SortAndGroupFields::Date);
    sortProperty.category = Category::FileCategory;
    sortProperty.type = SortProperty::Type::DateType;
    m_availableSortProperties->appendSortProperty(sortProperty);

    sortProperty.text = i18nc("@label:button file size", "File Size");
    sortProperty.sort = static_cast<int>(SortAndGroupFields::FileSize);
    sortProperty.category = Category::FileCategory;
    sortProperty.type = SortProperty::Type::IntType;
    m_availableSortProperties->appendSortProperty(sortProperty);

    sortProperty.text = i18nc("@label:button title", "Title");
    sortProperty.sort = static_cast<int>(SortAndGroupFields::Title);
    sortProperty.category = Category::FileCategory;
    sortProperty.type = SortProperty::Type::StringType;
    m_availableSortProperties->appendSortProperty(sortProperty);

    sortProperty.text = i18nc("@label:button, as in 'Track no on a Audio CD', not 'subtitle track'", "Track No");
    sortProperty.sort = static_cast<int>(SortAndGroupFields::TrackNo);
    sortProperty.category = Category::AudioCategory;
    sortProperty.type = SortProperty::Type::IntType;
    m_availableSortProperties->appendSortProperty(sortProperty);

    sortProperty.text = i18nc("@label:button sample rate", "Sample Rate");
    sortProperty.sort = static_cast<int>(SortAndGroupFields::SampleRate);
    sortProperty.category = Category::AudioCategory;
    sortProperty.type = SortProperty::Type::IntType;
    m_availableSortProperties->appendSortProperty(sortProperty);

    sortProperty.text = i18nc("@label:button bitrate", "Bitrate");
    sortProperty.sort = static_cast<int>(SortAndGroupFields::Bitrate);
    sortProperty.category = Category::AudioCategory;
    sortProperty.type = SortProperty::Type::IntType;
    m_availableSortProperties->appendSortProperty(sortProperty);

    // Set available grouping properties
    sortProperty.text = i18nc("@label:button folder", "Folder");
    sortProperty.sort = static_cast<int>(SortAndGroupFields::Folder);
    sortProperty.category = Category::FileCategory;
    sortProperty.type = SortProperty::Type::StringType;
    m_availableSortProperties->appendSortProperty(sortProperty);
    m_availableGroups->appendSortProperty(sortProperty);

    sortProperty.text = i18nc("@label:button file type", "Type");
    sortProperty.sort = static_cast<int>(SortAndGroupFields::FileType);
    sortProperty.category = Category::FileCategory;
    sortProperty.type = SortProperty::Type::StringType;
    m_availableSortProperties->appendSortProperty(sortProperty);
    m_availableGroups->appendSortProperty(sortProperty);

    sortProperty.text = i18nc("@label:button file extension", "Extension");
    sortProperty.sort = static_cast<int>(SortAndGroupFields::Extension);
    sortProperty.category = Category::FileCategory;
    sortProperty.type = SortProperty::Type::StringType;
    m_availableSortProperties->appendSortProperty(sortProperty);
    m_availableGroups->appendSortProperty(sortProperty);

    sortProperty.text = i18nc("@label:button disc no", "Disc No");
    sortProperty.sort = static_cast<int>(SortAndGroupFields::DiscNo);
    sortProperty.category = Category::AudioCategory;
    sortProperty.type = SortProperty::Type::IntType;
    m_availableSortProperties->appendSortProperty(sortProperty);
    m_availableGroups->appendSortProperty(sortProperty);

    sortProperty.text = i18nc("@label:button release year", "Release Year");
    sortProperty.sort = static_cast<int>(SortAndGroupFields::ReleaseYear);
    sortProperty.category = Category::AudioCategory;
    sortProperty.type = SortProperty::Type::IntType;
    m_availableSortProperties->appendSortProperty(sortProperty);
    m_availableGroups->appendSortProperty(sortProperty);

    sortProperty.text = i18nc("@label:button genre", "Genre");
    sortProperty.sort = static_cast<int>(SortAndGroupFields::Genre);
    sortProperty.category = Category::AudioCategory;
    sortProperty.type = SortProperty::Type::StringType;
    m_availableSortProperties->appendSortProperty(sortProperty);
    m_availableGroups->appendSortProperty(sortProperty);

    sortProperty.text = i18nc("@label:button album", "Album");
    sortProperty.sort = static_cast<int>(SortAndGroupFields::Album);
    sortProperty.category = Category::AudioCategory;
    sortProperty.type = SortProperty::Type::StringType;
    m_availableSortProperties->appendSortProperty(sortProperty);
    m_availableGroups->appendSortProperty(sortProperty);

    sortProperty.text = i18nc("@label:button artist", "Artist");
    sortProperty.sort = static_cast<int>(SortAndGroupFields::Artist);
    sortProperty.category = Category::AudioCategory;
    sortProperty.type = SortProperty::Type::StringType;
    m_availableSortProperties->appendSortProperty(sortProperty);
    m_availableGroups->appendSortProperty(sortProperty);

    sortProperty.text = i18nc("@label:button album artist", "Album Artist");
    sortProperty.sort = static_cast<int>(SortAndGroupFields::AlbumArtist);
    sortProperty.category = Category::AudioCategory;
    sortProperty.type = SortProperty::Type::StringType;
    m_availableSortProperties->appendSortProperty(sortProperty);
    m_availableGroups->appendSortProperty(sortProperty);

    sortProperty.text = i18nc("@label:button composer", "Composer");
    sortProperty.sort = static_cast<int>(SortAndGroupFields::Composer);
    sortProperty.category = Category::AudioCategory;
    sortProperty.type = SortProperty::Type::StringType;
    m_availableSortProperties->appendSortProperty(sortProperty);
    m_availableGroups->appendSortProperty(sortProperty);

    sortProperty.text = i18nc("@label:button lyricist", "Lyricist");
    sortProperty.sort = static_cast<int>(SortAndGroupFields::Lyricist);
    sortProperty.category = Category::AudioCategory;
    sortProperty.type = SortProperty::Type::StringType;
    m_availableSortProperties->appendSortProperty(sortProperty);
    m_availableGroups->appendSortProperty(sortProperty);

    sortProperty.text = i18nc("@label:button audio codec", "Audio Codec");
    sortProperty.sort = static_cast<int>(SortAndGroupFields::AudioCodec);
    sortProperty.category = Category::AudioCategory;
    sortProperty.type = SortProperty::Type::StringType;
    m_availableSortProperties->appendSortProperty(sortProperty);
    m_availableGroups->appendSortProperty(sortProperty);

    sortProperty.text = i18nc("@label:button framerate", "Framerate");
    sortProperty.sort = static_cast<int>(SortAndGroupFields::Framerate);
    sortProperty.category = Category::VideoCategory;
    sortProperty.type = SortProperty::Type::RealType;
    m_availableSortProperties->appendSortProperty(sortProperty);
    m_availableGroups->appendSortProperty(sortProperty);

    sortProperty.text = i18nc("@label:button video codec", "Video Codec");
    sortProperty.sort = static_cast<int>(SortAndGroupFields::VideoCodec);
    sortProperty.category = Category::VideoCategory;
    sortProperty.type = SortProperty::Type::StringType;
    m_availableSortProperties->appendSortProperty(sortProperty);
    m_availableGroups->appendSortProperty(sortProperty);

    sortProperty.text = i18nc("@label:button video orientation", "Orientation");
    sortProperty.sort = static_cast<int>(SortAndGroupFields::Orientation);
    sortProperty.category = Category::VideoCategory;
    sortProperty.type = SortProperty::Type::IntType;
    m_availableSortProperties->appendSortProperty(sortProperty);
    m_availableGroups->appendSortProperty(sortProperty);

    sortProperty.text = i18nc("@label:button resolution", "Resolution");
    sortProperty.sort = static_cast<int>(SortAndGroupFields::Resolution);
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
        return {};
    }

    // Only handle Section role here
    if (role == PlaylistModel::Roles::SectionRole) {
        if (m_indexToSection.empty()) {
            return {};
        }

        QSharedPointer sectionPtr = m_indexToSection.value(index.row());
        if (!m_showSections || sectionPtr.isNull()) {
            return {};
        }
        return *sectionPtr.data();
    }

    return sourceModel()->data(mapToSource(index), role);
}

bool PlaylistSortProxyModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    // Compare each group role in order
    const auto properties = m_activeSortProperties->properties();
    for (const auto &sortProperty : properties) {
        int compare = 0;

        // Handle special comparison cases
        if (sortProperty.sort == static_cast<int>(SortAndGroupFields::Resolution)) {
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
                return compare < 0;
            case PlaylistSortPropertyModel::Ascending: {
                return compare < 0;
            }
            case PlaylistSortPropertyModel::Descending: {
                return compare > 0;
            }
            }
        }
    }

    if (m_sortPreset == SortAndGroupFields::None) {
        switch (m_sortingOrder) {
        case Qt::AscendingOrder:
            return source_left.row() < source_right.row();
        case Qt::DescendingOrder:
            return source_left.row() > source_right.row();
        }
    }
    return source_left.row() < source_right.row(); // fallback
}

void PlaylistSortProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    QSortFilterProxyModel::setSourceModel(sourceModel);
    auto *playlistModel = qobject_cast<PlaylistModel *>(sourceModel);

    // When a new item is inserted, recreate sections with the new metadata after it is ready
    connect(playlistModel, &PlaylistModel::metaDataReady, this, &PlaylistSortProxyModel::scheduleSectionRecreation);
    connect(this, &QAbstractItemModel::rowsInserted, this, &PlaylistSortProxyModel::scheduleSectionRecreation);
    connect(this, &QAbstractItemModel::rowsRemoved, this, &PlaylistSortProxyModel::scheduleSectionRecreation);
}

PlaylistSortProxyModel::SortAndGroupFields PlaylistSortProxyModel::sortPreset() const
{
    return m_sortPreset;
}

void PlaylistSortProxyModel::setSortPreset(SortAndGroupFields preset)
{
    m_sortPreset = preset;
    qsizetype size = m_activeSortProperties->properties().size();
    for (auto i = 0; i < size; ++i) {
        removeFromActiveSortProperties(0);
    }

    if (preset != SortAndGroupFields::None) {
        addToActiveSortProperties(preset);
    }
    sortItems();
}

void PlaylistSortProxyModel::setSortOrder(Qt::SortOrder order)
{
    m_sortingOrder = order;
    sortItems();
    recreateSections();
}

void PlaylistSortProxyModel::sortItems()
{
    invalidate();
    // Note that Qt::SortOrder::AscendingOrder is given here regardless of m_sortingOrder
    // because  the order will be handled inside custom lessThan() method.
    sort(0, Qt::SortOrder::AscendingOrder);
}

Qt::SortOrder PlaylistSortProxyModel::sortingOrder() const
{
    return m_sortingOrder;
}

void PlaylistSortProxyModel::addToActiveSortProperties(SortAndGroupFields sort)
{
    SortProperty property = m_availableSortProperties->takeSortProperty(static_cast<int>(sort));
    m_activeSortProperties->appendSortProperty(property);
}

void PlaylistSortProxyModel::addToActiveGroup(SortAndGroupFields group)
{
    if (group == SortAndGroupFields::Separator) {
        SortProperty sortProperty;
        sortProperty.text = i18nc("@label:button put new line", "New Line");
        sortProperty.category = Category::SeparatorCategory;
        sortProperty.sort = static_cast<int>(SortAndGroupFields::Separator);
        m_activeGroups->appendSortProperty(sortProperty);
        return;
    }
    SortProperty property = m_availableGroups->takeSortProperty(static_cast<int>(group));
    m_activeGroups->appendSortProperty(property);
}

void PlaylistSortProxyModel::removeFromActiveSortProperties(uint index)
{
    SortProperty property = m_activeSortProperties->properties().at(index);
    m_activeSortProperties->removeProperty(index);
    m_availableSortProperties->appendSortProperty(property);
}

void PlaylistSortProxyModel::removeFromActiveGroup(uint index)
{
    SortProperty property = m_activeGroups->properties().at(index);
    m_activeGroups->removeProperty(index);
    if (property.sort == static_cast<int>(SortAndGroupFields::Separator)) {
        return;
    }
    m_availableGroups->appendSortProperty(property);
}

void PlaylistSortProxyModel::onActiveSortPropertiesChanged()
{
    const auto properties = m_activeSortProperties->properties();
    if (properties.isEmpty()) {
        m_sortPreset = SortAndGroupFields::None;
        sortItems();
        recreateSections();
        return;
    }
    // Check if the change is a preset
    if (properties.size() == 1) {
        int preset = properties.first().sort;

        switch (static_cast<SortAndGroupFields>(preset)) {
        case SortAndGroupFields::FileName:
        case SortAndGroupFields::Duration:
        case SortAndGroupFields::Date:
        case SortAndGroupFields::FileSize:
        case SortAndGroupFields::Title:
        case SortAndGroupFields::TrackNo:
        case SortAndGroupFields::SampleRate:
        case SortAndGroupFields::Bitrate: {
            m_sortPreset = SortAndGroupFields(preset);
            break;
        }
        default:
            m_sortPreset = SortAndGroupFields::Custom;
            break;
        }
    } else {
        m_sortPreset = SortAndGroupFields::Custom;
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
    m_activeGroups->setHideBlank(index, hide);
    onActiveGroupsChanged();
}

void PlaylistSortProxyModel::setSortPropertySortingOrder(uint index, int order)
{
    m_activeSortProperties->setOrder(index, order);
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

        if (a < b) {
            return -1;
        }
        if (a > b) {
            return 1;
        }
        return 0;
    }
    case SortProperty::RealType: {
        double a = l.toDouble();
        double b = r.toDouble();
        if (a < b) {
            return -1;
        }
        if (a > b) {
            return 1;
        }
        return 0;
    }
    case SortProperty::DateType: {
        QDateTime a = l.toDateTime();
        QDateTime b = r.toDateTime();
        if (a < b) {
            return -1;
        }
        if (a > b) {
            return 1;
        }
        return 0;
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

    if (!m_showSections || m_activeGroups->properties().isEmpty()) {
        Q_EMIT dataChanged(index(0, 0), index(rowCount() - 1, 0));
        Q_EMIT groupingChanged();
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
    Q_EMIT groupingChanged();
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
    QHash<SortAndGroupFields, QString> placeholders = {
        {SortAndGroupFields::Genre,         i18nc("@title:group", "No Genre")},
        {SortAndGroupFields::Album,         i18nc("@title:group", "No Album")},
        {SortAndGroupFields::Artist,        i18nc("@title:group", "No Artist")},
        {SortAndGroupFields::AlbumArtist,   i18nc("@title:group", "No Album Artist")},
        {SortAndGroupFields::Composer,      i18nc("@title:group", "No Composer")},
        {SortAndGroupFields::Lyricist,      i18nc("@title:group", "No Lyricist")},
        {SortAndGroupFields::ReleaseYear,   i18nc("@title:group", "No Release Year")},
        {SortAndGroupFields::Resolution,    i18nc("@title:group", "Not a Video")},
        {SortAndGroupFields::AudioCodec,    i18nc("@title:group", "No Audio Codec")},
        {SortAndGroupFields::VideoCodec,    i18nc("@title:group", "No Video Codec")},
    };
    // clang-format on

    const auto properties = m_activeGroups->properties();
    for (const auto &sortProperty : properties) {
        bool hideBlank = sortProperty.hideBlank; // show in the section if and only if it exists

        switch (SortAndGroupFields(sortProperty.sort)) {
        case SortAndGroupFields::Folder: {
            QString folderPath = sourceModel()->data(mapToSource(index), sortProperty.sort).toString();
            currentSectionRow.append(folderPath);
            hideBlankRow.append(hideBlank);
            break;
        }
        case SortAndGroupFields::Extension: {
            QString extension = sourceModel()->data(mapToSource(index), sortProperty.sort).toString();
            currentSectionRow.append(extension);
            hideBlankRow.append(hideBlank);
            break;
        }
        case SortAndGroupFields::DiscNo: {
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
        case SortAndGroupFields::ReleaseYear: {
            int releaseYear = sourceModel()->data(mapToSource(index), sortProperty.sort).toInt();
            if (releaseYear <= 0) {
                if (!hideBlank) {
                    QString placeholder = placeholders.value(SortAndGroupFields(sortProperty.sort));
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
        case SortAndGroupFields::Framerate: {
            double fps = sourceModel()->data(mapToSource(index), sortProperty.sort).toDouble();
            int fpsDecimal = static_cast<int>(fps);
            if (fps > fpsDecimal) {
                // Values like 29.97 or 59.94. Show them as float
                currentSectionRow.append(QString::number(fps, 'f', 3) + u"fps");
            } else {
                currentSectionRow.append(QString::number(fpsDecimal) + u"fps");
            }
            hideBlankRow.append(hideBlank);
            break;
        }
        case SortAndGroupFields::Resolution: {
            int height = sourceModel()->data(mapToSource(index), PlaylistModel::DisplayedHeightRole).toInt();
            int width = sourceModel()->data(mapToSource(index), PlaylistModel::DisplayedWidthRole).toInt();
            if (height == 0 || width == 0) {
                if (!hideBlank) {
                    QString placeholder = placeholders.value(SortAndGroupFields(sortProperty.sort));
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
        case SortAndGroupFields::FileType: {
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
        case SortAndGroupFields::AudioCodec:
        case SortAndGroupFields::VideoCodec: {
            QString codec = sourceModel()->data(mapToSource(index), sortProperty.sort).toString();
            if (codec.isEmpty()) {
                if (!hideBlank) {
                    QString placeholder = placeholders.value(SortAndGroupFields(sortProperty.sort));
                    currentSectionRow.append(placeholder);
                    hideBlankRow.append(hideBlank);
                }
                break;
            }
            currentSectionRow.append(codec);
            hideBlankRow.append(hideBlank);
            break;
        }
        case SortAndGroupFields::Orientation: {
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
        case SortAndGroupFields::Genre:
        case SortAndGroupFields::Album:
        case SortAndGroupFields::Artist:
        case SortAndGroupFields::AlbumArtist:
        case SortAndGroupFields::Composer:
        case SortAndGroupFields::Lyricist: {
            const auto sgField = static_cast<int>(SortAndGroupFields(sortProperty.sort));
            QString itemString = sourceModel()->data(mapToSource(index), sgField).toString();
            if (itemString.isEmpty()) {
                if (!hideBlank) {
                    QString placeholder = placeholders.value(SortAndGroupFields(sortProperty.sort));
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
                if (hideBlankRow.at(itemIndex)) {
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
        case SortAndGroupFields::Separator: {
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
        case SortAndGroupFields::None:
        case SortAndGroupFields::Custom:
        case SortAndGroupFields::FileName:
        case SortAndGroupFields::Duration:
        case SortAndGroupFields::Date:
        case SortAndGroupFields::FileSize:
        case SortAndGroupFields::Title:
        case SortAndGroupFields::TrackNo:
        case SortAndGroupFields::SampleRate:
        case SortAndGroupFields::Bitrate:
            return {};
        }
    }

    if (!currentSectionRow.isEmpty()) {
        sectionRows.append(currentSectionRow.join(u" · "));
    }

    return sectionRows;
}

QHash<QString, QStringList> PlaylistSortProxyModel::sectionMap() const
{
    return m_sectionMap;
}

PlaylistSortPropertyProxyModel *PlaylistSortProxyModel::availableGroupsProxy() const
{
    return m_availableGroupsProxy.get();
}

PlaylistSortPropertyProxyModel *PlaylistSortProxyModel::availableSortPropertiesProxy() const
{
    return m_availableSortPropertiesProxy.get();
}

PlaylistSortPropertyModel *PlaylistSortProxyModel::activeGroups() const
{
    return m_activeGroups.get();
}

PlaylistSortPropertyModel *PlaylistSortProxyModel::activeSortProperties() const
{
    return m_activeSortProperties.get();
}

bool PlaylistSortProxyModel::showSections() const
{
    return m_showSections;
}

void PlaylistSortProxyModel::setShowSections(bool newShowSections)
{
    m_showSections = newShowSections;
}

#include "moc_playlistsortproxymodel.cpp"
