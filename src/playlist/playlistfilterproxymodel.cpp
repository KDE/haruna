/*
 * SPDX-FileCopyrightText: 2025 George Florea Bănuș <georgefb899@gmail.com>
 * SPDX-FileCopyrightText: 2025 Muhammet Sadık Uğursoy <sadikugursoy@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "playlistfilterproxymodel.h"

#include <QClipboard>
#include <QCollator>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QGuiApplication>
#include <QMap>
#include <QTimer>

#include <KFileItem>
#include <KIO/DeleteOrTrashJob>
#include <KIO/RenameFileDialog>

#include "miscutils.h"
#include "pathutils.h"
#include "playlistsettings.h"

using namespace Qt::StringLiterals;
PlaylistFilterProxyModel::PlaylistFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel{parent}
    , m_playlistModel{std::make_unique<PlaylistModel>()}
    , m_playlistSortProxyModel{std::make_unique<PlaylistSortProxyModel>()}
    , m_playlistProxyModel{std::make_unique<PlaylistProxyModel>()}
    , m_selectionModel(this)
{
    m_playlistSortProxyModel->setSourceModel(m_playlistModel.get());
    m_playlistProxyModel->setSourceModel(m_playlistSortProxyModel.get());
    setSourceModel(m_playlistProxyModel.get());

    setDynamicSortFilter(true);
    setFilterCaseSensitivity(Qt::CaseInsensitive);
    invalidate();

    connect(&m_selectionModel, &QItemSelectionModel::selectionChanged, this, &PlaylistFilterProxyModel::onSelectionChanged);
    connect(this, &QSortFilterProxyModel::rowsInserted, this, &PlaylistFilterProxyModel::shufflePlaylistModel);
    connect(this, &QSortFilterProxyModel::rowsRemoved, this, &PlaylistFilterProxyModel::shufflePlaylistModel);
}

QVariant PlaylistFilterProxyModel::data(const QModelIndex &index, int role) const
{
    if (role == PlaylistModel::IsSelectedRole) {
        return QVariant(m_selectionModel.isSelected(index));
    }
    return sourceModel()->data(mapToSource(index), role);
}

uint PlaylistFilterProxyModel::selectionCount()
{
    return m_selectionModel.selectedRows().count();
}

uint PlaylistFilterProxyModel::itemCount()
{
    return rowCount();
}

QString PlaylistFilterProxyModel::searchText()
{
    return filterRegularExpression().pattern();
}

void PlaylistFilterProxyModel::setSearchText(QString text)
{
    Filter filter = PlaylistSettings::showMediaTitle() ? Filter::Title : Filter::Name;
    setFilterRole(filter);
    setFilterRegularExpression(text);
    invalidateFilter();
    Q_EMIT searchTextChanged();
}

Sort PlaylistFilterProxyModel::sortPreset()
{
    return playlistSortProxyModel()->m_sortPreset;
}

void PlaylistFilterProxyModel::setSortPreset(PlaylistSortProxyModel::Sort pSortRole)
{
    if (sortPreset() == pSortRole) {
        // Reset custom ordering
        playlistProxyModel()->onLayoutAboutToBeChanged();
        return;
    }
    playlistSortProxyModel()->setSortPreset(pSortRole);
    Q_EMIT itemsSorted();
}

Qt::SortOrder PlaylistFilterProxyModel::sortOrder()
{
    return playlistSortProxyModel()->m_sortingOrder;
}

void PlaylistFilterProxyModel::changeSortOrder(Qt::SortOrder order)
{
    if (sortOrder() == order) {
        // Reset custom ordering
        playlistProxyModel()->onLayoutAboutToBeChanged();
        return;
    }
    playlistSortProxyModel()->setSortOrder(order);
    Q_EMIT itemsSorted();
    Q_EMIT sortOrderChanged();
}

bool PlaylistFilterProxyModel::showSections()
{
    return playlistSortProxyModel()->m_showSections;
}

void PlaylistFilterProxyModel::setShowSections(bool split)
{
    if (playlistSortProxyModel()->m_showSections == split) {
        return;
    }
    playlistSortProxyModel()->m_showSections = split;
    playlistSortProxyModel()->recreateSections();
    Q_EMIT showSectionsChanged();
}

uint PlaylistFilterProxyModel::getPlayingItem()
{
    auto model = playlistModel();
    return mapFromPlaylistModel(model->m_playingItem).row();
}

void PlaylistFilterProxyModel::setPlayingItem(uint i)
{
    auto model = playlistModel();
    model->setPlayingItem(mapToPlaylistModel(i).row());
}

void PlaylistFilterProxyModel::playNext()
{
    auto model = playlistModel();
    if (PlaylistSettings::randomPlayback()) {
        auto nextIndex = model->currentShuffledIndex() + 1;
        if (nextIndex >= static_cast<int>(model->shuffledIndexes().size())) {
            nextIndex = 0;
        }
        auto shuffledIndex = model->shuffledIndexes().at(nextIndex);
        model->setCurrentShuffledIndex(nextIndex);
        model->setPlayingItem(shuffledIndex);
    } else {
        auto currentIndex = mapFromPlaylistModel(model->m_playingItem).row();
        auto nextIndex = currentIndex + 1;
        if (nextIndex < rowCount()) {
            setPlayingItem(nextIndex);
        } else {
            setPlayingItem(0);
        }
    }
}

void PlaylistFilterProxyModel::playPrevious()
{
    auto model = playlistModel();
    if (PlaylistSettings::randomPlayback()) {
        auto previousIndex = model->currentShuffledIndex() - 1;
        if (previousIndex < 0) {
            previousIndex = model->shuffledIndexes().size() - 1;
        }
        auto shuffledIndex = model->shuffledIndexes().at(previousIndex);
        model->setCurrentShuffledIndex(previousIndex);
        model->setPlayingItem(shuffledIndex);
    } else {
        auto currentIndex = mapFromPlaylistModel(model->m_playingItem).row();
        auto previousIndex = currentIndex - 1;

        if (previousIndex >= 0) {
            setPlayingItem(index(previousIndex, 0).row());
        } else {
            setPlayingItem(index(rowCount() - 1, 0).row());
        }
    }
}

void PlaylistFilterProxyModel::saveM3uFile(const QString &path)
{
    QUrl url(path);
    QFile m3uFile(url.toString(QUrl::PreferLocalFile));
    if (!m3uFile.open(QFile::WriteOnly)) {
        return;
    }
    for (int i{0}; i < playlistProxyModel()->rowCount(); ++i) {
        QString itemPath = playlistProxyModel()->data(playlistProxyModel()->index(i, 0), PlaylistModel::PathRole).toString();
        m3uFile.write(itemPath.toUtf8().append("\n"));
    }
    m3uFile.close();
}

void PlaylistFilterProxyModel::highlightInFileManager(uint row)
{
    QString path = data(index(row, 0), PlaylistModel::PathRole).toString();
    PathUtils::instance()->highlightInFileManager(path);
}

void PlaylistFilterProxyModel::removeItem(uint row)
{
    auto model = playlistModel();
    model->removeItem(mapToPlaylistModel(row).row());
    Q_EMIT itemsRemoved();
    Q_EMIT itemCountChanged();
}

void PlaylistFilterProxyModel::removeItems()
{
    QList<int> rows;
    QModelIndexList selected = selectedRows();
    for (auto it = selected.begin(); it != selected.end(); ++it) {
        auto sourceRow = mapToPlaylistModel(it->row()).row();
        rows.append(sourceRow);
    }
    // Need to remove items from bottom to top, in order not to mess with the indices
    std::sort(rows.begin(), rows.end(), std::greater<>());
    auto model = playlistModel();
    for (auto row : std::as_const(rows)) {
        model->removeItem(row);
    }
    Q_EMIT itemsRemoved();
    Q_EMIT itemCountChanged();
}

void PlaylistFilterProxyModel::renameFile(uint row)
{
    QString path = data(index(row, 0), PlaylistModel::PathRole).toString();
    QUrl url(path);
    if (url.scheme().isEmpty()) {
        url.setScheme(u"file"_s);
    }
    KFileItem item(url);
    auto renameDialog = new KIO::RenameFileDialog(KFileItemList({item}), nullptr);
    renameDialog->open();

    connect(renameDialog, &KIO::RenameFileDialog::renamingFinished, this, [=](const QList<QUrl> &urls) {
        auto model = playlistModel();
        auto sourceRow = mapToPlaylistModel(row).row();
        auto &item = model->m_playlist.at(sourceRow);
        item.url = QUrl::fromUserInput(urls.first().path());
        item.filename = urls.first().fileName();

        Q_EMIT dataChanged(index(row, 0), index(row, 0));
    });
}

void PlaylistFilterProxyModel::trashFile(uint row)
{
    QList<QUrl> urls;
    QString path = data(index(row, 0), PlaylistModel::PathRole).toString();
    QUrl url(path);
    if (url.scheme().isEmpty()) {
        url.setScheme(u"file"_s);
    }
    urls << url;
    auto *job = new KIO::DeleteOrTrashJob(urls, KIO::AskUserActionInterface::Trash, KIO::AskUserActionInterface::DefaultConfirmation, this);
    job->start();

    connect(job, &KJob::result, this, [=]() {
        if (job->error() == 0) {
            auto model = playlistModel();
            auto sourceRow = mapToPlaylistModel(row).row();
            model->removeItem(sourceRow);
        }
    });
}

void PlaylistFilterProxyModel::trashFiles()
{
    QList<QUrl> urls;
    QList<int> rows;

    QModelIndexList selected = selectedRows();
    for (auto it = selected.cbegin(); it != selected.cend(); ++it) {
        QString path = data(index(it->row(), 0), PlaylistModel::PathRole).toString();
        QUrl url(path);
        if (url.scheme().isEmpty()) {
            url.setScheme(u"file"_s);
        }
        auto sourceRow = mapToPlaylistModel(it->row()).row();
        rows.append(sourceRow);
        urls << url;
    }
    // Need to remove items from bottom to top, in order not to mess with the indices
    std::sort(rows.begin(), rows.end(), std::greater<>());

    auto *job = new KIO::DeleteOrTrashJob(urls, KIO::AskUserActionInterface::Trash, KIO::AskUserActionInterface::DefaultConfirmation, this);
    job->start();

    connect(job, &KJob::result, this, [=]() {
        if (job->error() == 0) {
            auto model = playlistModel();
            for (auto row : std::as_const(rows)) {
                model->removeItem(row);
            }
        }
    });
}

void PlaylistFilterProxyModel::copyFileName(uint row)
{
    auto model = playlistModel();
    auto sourceRow = mapToPlaylistModel(row).row();
    auto item = model->m_playlist.at(sourceRow);
    QGuiApplication::clipboard()->setText(item.filename);
}

void PlaylistFilterProxyModel::copyFilePath(uint row)
{
    auto model = playlistModel();
    auto sourceRow = mapToPlaylistModel(row).row();
    auto item = model->m_playlist.at(sourceRow);
    QGuiApplication::clipboard()->setText(item.url.toString());
}

QString PlaylistFilterProxyModel::getFilePath(uint row)
{
    auto model = playlistModel();
    auto sourceRow = mapToPlaylistModel(row).row();
    auto item = model->m_playlist.at(sourceRow);
    return item.url.toString();
}

bool PlaylistFilterProxyModel::isLastItem(uint row)
{
    if (PlaylistSettings::randomPlayback()) {
        return static_cast<int>(row) == playlistModel()->shuffledIndexes().back();
    }
    return static_cast<int>(row) == rowCount() - 1;
}

void PlaylistFilterProxyModel::moveItems(uint row, uint destinationRow)
{
    // Prevent an invalid move
    if (destinationRow == row) {
        return;
    }
    // Prevent out of bound moves
    if (static_cast<int>(row) > rowCount() || static_cast<int>(destinationRow) > rowCount()) {
        return;
    }

    QModelIndexList selection = selectedRows();

    // Check whether the drag move is top-down or bottom-up. Qt::beginMoveRows always put the items on top of
    // the destination row, but this is not what we always want so we need to take it into account
    bool topDownDrag = destinationRow > row;

    // Split the selection ranges into Top and Bottom parts, with respect to the destination row.
    // This makes managing the move operations easier for the PlaylistProxyModel layout.
    QModelIndexList topSelection;
    QModelIndexList bottomSelection;
    splitItemSelection(selection, destinationRow, topDownDrag, topSelection, bottomSelection);

    /* Playlist: A, B, C, D, E
     * Selected: A, C, E
     * Dragged item: C
     *
     * Drag Over:
     *     B -> topDownDrag = true  -> topSelection = A,   bottomSelection = C, E
     *     D -> topDownDrag = false -> topSelection = A, C bottomSelection = E
     *
     * A or E can be a chunk of selection, it doesn't matter
     */

    std::vector<uint> updatedSelectedRows;
    int rowOffset = 0;
    for (const auto &iTopSelection : std::as_const(topSelection)) {
        // These indices are located above the destination. Each move will reduce index of the
        // following rows by 1 until the destination row since they are sorted in ascending order.
        // We take this into account by using rowOffset
        int first = iTopSelection.row();
        int destination = topDownDrag ? destinationRow : destinationRow - 1;

        /* Playlist: A, B, C, D, E
         * Selected: A, B, D
         * Selected indices: 0, 1, 3
         * Dragged on: E (topDownDrag true in this case)
         * Destination index: 4 (not 3 since topDownDrag is true)
         *  Order of operations will be as follows:
         *
         *  (1)     (2)     (3)     (4)
         *  0. A    0. B    0. C    0. C
         *  1. B    1. C    1. D    1. E
         *  2. C -> 2. D -> 2. E -> 2. A
         *  3. D    3. E    3. A    3. B
         *  4. E    4. A    4. B    4. D
         *
         * Index movement: (1) 0->4, (2) 0->4, (3) 1->4
         * Indices 0, 1, 3 are decreased by 1 after each item move operation
         * so, do (index - offset) for each item to move them properly
         *
         * The final selection is: 2, 3, 4
         * That is (destinationIndex - itemCount + N) for Nth item
         */

        // Qt expects the movement to be on top of the destination row, which is not the case for
        // topSelection rows for THIS logic, since we set it by considering the Proxy's layout vector.
        // Therefore, we need to increase the index of the destination by 1 here. See:
        // https://doc.qt.io/qt-6/qabstractitemmodel.html#beginMoveRows
        if (beginMoveRows(QModelIndex(), first - rowOffset, first - rowOffset, QModelIndex(), destination + 1)) {
            int sourceFirst = mapToSource(index(first, 0)).row();
            int sourceDestination = mapToSource(index(destination, 0)).row();
            playlistProxyModel()->moveRows(QModelIndex(), sourceFirst - rowOffset, 1, QModelIndex(), sourceDestination);
            endMoveRows();
        }
        updatedSelectedRows.push_back(destination - topSelection.size() + rowOffset + 1);
        rowOffset++;
    }
    rowOffset = 0;
    for (const auto &iBottomSelection : std::as_const(bottomSelection)) {
        // Indices here will be located below the destination. Moving the items to the top will not change
        // their indices, but it will change the target index. We take this into account in the same way.
        int first = iBottomSelection.row();
        int destination = topDownDrag ? destinationRow + 1 : destinationRow;

        /* Playlist: A, B, C, D, E
         * Selected: B, D, E
         * Selected indices: 1, 3, 4
         * Dragged on: A (topDownDrag false in this case)
         * Destination index: 0 (not 1 since topDownDrag is false)
         *  Order of operations will be as follows:
         *
         *  (1)     (2)     (3)     (4)
         *  0. A    0. B    0. B    0. B
         *  1. B    1. A    1. D    1. D
         *  2. C -> 2. C -> 2. A -> 2. E
         *  3. D    3. D    3. C    3. A
         *  4. E    4. E    4. E    4. C
         *
         * Index movements (1) 1->0, (2) 3->1, (3) 4->2
         * Indices 1, 3, 4 are the same but destination is increased by 1 after each item move operation
         * so, do (destination + offset) for each item
         *
         * The final selection is: 0, 1, 2
         * That is (destinationIndex + offset) for each item
         */

        if (beginMoveRows(QModelIndex(), first, first, QModelIndex(), destination + rowOffset)) {
            int sourceFirst = mapToSource(index(first, 0)).row();
            int sourceDestination = mapToSource(index(destination, 0)).row();
            playlistProxyModel()->moveRows(QModelIndex(), sourceFirst, 1, QModelIndex(), sourceDestination + rowOffset);
            endMoveRows();
        }
        updatedSelectedRows.push_back(destination + rowOffset);
        rowOffset++;
    }

    invalidateRowsFilter();
    QItemSelection updatedSelection;
    for (const auto &row : std::as_const(updatedSelectedRows)) {
        updatedSelection.select(index(row, 0), index(row, 0));
    }
    m_selectionModel.select(updatedSelection, QItemSelectionModel::SelectionFlag::Select | QItemSelectionModel::SelectionFlag::Clear);

    Q_EMIT itemsMoved();
}

void PlaylistFilterProxyModel::selectItem(uint row, Selection selectionMode)
{
    if (static_cast<int>(row) >= rowCount()) {
        return;
    }

    switch (selectionMode) {
    case Selection::Clear:
        m_selectionModel.clearSelection();
        break;
    case Selection::ClearSingle:
        m_selectionModel.select(index(row, 0), QItemSelectionModel::SelectionFlag::Select | QItemSelectionModel::SelectionFlag::Clear);
        m_selectionModel.setCurrentIndex(index(row, 0), QItemSelectionModel::SelectionFlag::NoUpdate);
        break;
    case Selection::Single:
        if (m_selectionModel.isSelected(index(row, 0))) {
            return;
        }
        m_selectionModel.select(index(row, 0), QItemSelectionModel::SelectionFlag::Select | QItemSelectionModel::SelectionFlag::Clear);
        m_selectionModel.setCurrentIndex(index(row, 0), QItemSelectionModel::SelectionFlag::NoUpdate);
        break;
    case Selection::Toggle:
        m_selectionModel.select(index(row, 0), QItemSelectionModel::SelectionFlag::Toggle);
        m_selectionModel.setCurrentIndex(index(row, 0), QItemSelectionModel::SelectionFlag::NoUpdate);
        break;
    case Selection::Range: {
        QItemSelection range;
        range.select(m_selectionModel.currentIndex(), index(row, 0));
        m_selectionModel.select(range, QItemSelectionModel::SelectionFlag::Select | QItemSelectionModel::SelectionFlag::Clear);
        break;
    }
    case Selection::RangeStart:
        m_selectionModel.setCurrentIndex(index(row, 0), QItemSelectionModel::SelectionFlag::NoUpdate);
        break;
    case Selection::Invert: {
        QItemSelection selectAll;
        selectAll.select(index(0, 0), index(rowCount() - 1, 0));
        m_selectionModel.select(selectAll, QItemSelectionModel::SelectionFlag::Toggle);
        break;
    }
    case Selection::All: {
        QItemSelection selectAll;
        selectAll.select(index(0, 0), index(rowCount() - 1, 0));
        m_selectionModel.select(selectAll, QItemSelectionModel::SelectionFlag::Select);
        break;
    }
    }
}

// Used by QML::ListView, inside onModelChanged. When the visible playlist is scrolled down and another
// tab is pressed, the new tab will use pooled items without updating them. This updates the list as whole.
void PlaylistFilterProxyModel::refreshData()
{
    Q_EMIT dataChanged(index(0, 0), index(rowCount() - 1, 0));
}

void PlaylistFilterProxyModel::addFilesAndFolders(QList<QUrl> urls, PlaylistModel::Behavior behavior, uint insertOffset)
{
    auto getCanonicalOrAbsolutePath = [](const QFileInfo &fi) -> QString {
        const QString canonical = fi.canonicalFilePath();
        if (canonical.isEmpty()) {
            return fi.absoluteFilePath();
        }
        return canonical;
    };

    auto isAcceptedMime = [](const QString &path) -> bool {
        QString mimeType = MiscUtils::mimeType(QUrl::fromLocalFile(path));
        if (mimeType == u"audio/x-mpegurl"_s) {
            return false;
        }
        return (mimeType.startsWith(u"video/") || mimeType.startsWith(u"audio/"));
    };

    QList<QFileInfo> dirs;
    QList<QString> files;
    QSet<QString> visitedPaths; // prevent infinite symlink loops
    for (const auto &url : urls) {
        QFileInfo fileInfo(url.toLocalFile());
        if (!fileInfo.exists()) {
            continue;
        }

        auto key = getCanonicalOrAbsolutePath(fileInfo);
        if (visitedPaths.contains(key)) {
            continue;
        }
        visitedPaths.insert(key);

        if (fileInfo.isDir()) {
            dirs.append(fileInfo);
            continue;
        }

        if (fileInfo.isFile()) {
            if (isAcceptedMime(url.toString())) {
                files.append(url.toLocalFile());
            }
            continue;
        }
    }

    for (const auto &dir : dirs) {
        QFileInfo fileInfo(dir);
        QDirIterator it(fileInfo.absoluteFilePath(), QDir::Files, QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);
        while (it.hasNext()) {
            QString file = it.next();
            QFileInfo fileInfo(file);

            if (!fileInfo.exists()) {
                continue;
            }

            auto key = getCanonicalOrAbsolutePath(fileInfo);
            if (visitedPaths.contains(key)) {
                continue;
            }
            visitedPaths.insert(key);

            if (isAcceptedMime(file)) {
                files.append(file);
            }
        }
    }

    QCollator collator;
    collator.setNumericMode(true);
    std::sort(files.begin(), files.end(), collator);

    if (behavior == PlaylistModel::Clear) {
        playlistModel()->clear();
    }

    int localOffset = 0;
    for (const auto &file : std::as_const(files)) {
        if (behavior == PlaylistModel::Insert) {
            // Initialize proxy model to insert the items at the dropped index. After each insertion, the index should increase
            playlistProxyModel()->setInsertOffset(insertOffset + localOffset++);
            // When the addItem is called from the playlistModel, rowsAboutToBeInserted signal will reach playlistProxyModel
            // for this item. If offset it set, instead of appending, the proxy will insert it to the given index
        }
        // PlaylistModel can just append the items
        playlistModel()->addItem(QUrl::fromLocalFile(file), PlaylistModel::Append);
    }
    Q_EMIT itemsInserted();
    Q_EMIT itemCountChanged();
}

bool PlaylistFilterProxyModel::isDirectory(const QUrl &url)
{
    QFileInfo fileInfo(url.toLocalFile());
    return fileInfo.exists() && fileInfo.isDir();
}

PlaylistSortPropertyModel *PlaylistFilterProxyModel::activeSortPropertiesModel()
{
    auto sortModel = playlistSortProxyModel();
    return sortModel->m_activeSortProperties.get();
}

PlaylistSortPropertyModel *PlaylistFilterProxyModel::activeGroupModel()
{
    auto sortModel = playlistSortProxyModel();
    return sortModel->m_activeGroups.get();
}

PlaylistSortPropertyProxyModel *PlaylistFilterProxyModel::availableSortPropertiesProxyModel()
{
    auto sortModel = playlistSortProxyModel();
    return sortModel->m_availableSortPropertiesProxy.get();
}

PlaylistSortPropertyProxyModel *PlaylistFilterProxyModel::availableGroupProxyModel()
{
    auto sortModel = playlistSortProxyModel();
    return sortModel->m_availableGroupsProxy.get();
}

void PlaylistFilterProxyModel::addToActiveSortProperties(int sort)
{
    playlistSortProxyModel()->addToActiveSortProperties(sort);
    Q_EMIT itemsSorted();
}

void PlaylistFilterProxyModel::addToActiveGroup(Group group) const
{
    playlistSortProxyModel()->addToActiveGroup(group);
}

void PlaylistFilterProxyModel::removeFromActiveSortProperties(uint index)
{
    playlistSortProxyModel()->removeFromActiveSortProperties(index);
    Q_EMIT itemsSorted();
}

void PlaylistFilterProxyModel::removeFromActiveGroup(uint index) const
{
    playlistSortProxyModel()->removeFromActiveGroup(index);
}

void PlaylistFilterProxyModel::setGroupHideBlank(uint index, bool hide) const
{
    playlistSortProxyModel()->setGroupHideBlank(index, hide);
}

void PlaylistFilterProxyModel::setSortPropertySortingOrder(uint index, int order) const
{
    // no need to map the index, active group properties are not sorted by a proxy
    playlistSortProxyModel()->setSortPropertySortingOrder(index, order);
}

QStringList PlaylistFilterProxyModel::getSectionList(QString sectionKey) const
{
    return playlistSortProxyModel()->m_sectionMap[sectionKey];
}

QString PlaylistFilterProxyModel::playlistName() const
{
    return playlistModel()->m_playlistName;
}

void PlaylistFilterProxyModel::clear()
{
    playlistModel()->clear();
    Q_EMIT itemsRemoved();
    Q_EMIT itemCountChanged();
}

void PlaylistFilterProxyModel::addItem(const QString &path, PlaylistModel::Behavior behavior)
{
    auto url = QUrl::fromUserInput(path);
    playlistModel()->addItem(url, behavior);
    Q_EMIT itemsInserted();
    Q_EMIT itemCountChanged();
}

void PlaylistFilterProxyModel::addItem(const QUrl &url, PlaylistModel::Behavior behavior)
{
    playlistModel()->addItem(url, behavior);
    Q_EMIT itemsInserted();
    Q_EMIT itemCountChanged();
}

void PlaylistFilterProxyModel::addItems(const QList<QUrl> &urls, PlaylistModel::Behavior behavior)
{
    for (const auto &url : urls) {
        playlistModel()->addItem(url, behavior);
    }
    Q_EMIT itemsInserted();
    Q_EMIT itemCountChanged();
}

void PlaylistFilterProxyModel::onSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    for (const auto &index : selected.indexes()) {
        Q_EMIT dataChanged(index, index);
    }
    for (const auto &index : deselected.indexes()) {
        Q_EMIT dataChanged(index, index);
    }
    Q_EMIT selectionCountChanged();
}

void PlaylistFilterProxyModel::shufflePlaylistModel()
{
    if (m_scheduledReshuffle) {
        return;
    }
    m_scheduledReshuffle = true;

    // rowsInserted/rowsRemoved signals will be called for each filtered item
    // this queues the shuffle event to be executed once at the end of the event loop
    QTimer::singleShot(0, this, [this]() {
        // Do the normal shuffle if filter is removed
        if (searchText().isEmpty()) {
            playlistModel()->shuffleIndexes();
            m_scheduledReshuffle = false;
            return;
        }

        std::vector<int> indices;
        for (int i = 0; i < rowCount(); ++i) {
            indices.push_back(mapToPlaylistModel(index(i, 0).row()).row());
        }
        playlistModel()->shuffleIndexes(indices);
        m_scheduledReshuffle = false;
    });
}

PlaylistProxyModel *PlaylistFilterProxyModel::playlistProxyModel() const
{
    return m_playlistProxyModel.get();
}

PlaylistSortProxyModel *PlaylistFilterProxyModel::playlistSortProxyModel() const
{
    return m_playlistSortProxyModel.get();
}

PlaylistModel *PlaylistFilterProxyModel::playlistModel() const
{
    return m_playlistModel.get();
}

QModelIndex PlaylistFilterProxyModel::mapFromPlaylistModel(uint row) const
{
    QModelIndex playlistIndex = playlistModel()->index(row, 0);
    QModelIndex sortProxyIndex = playlistSortProxyModel()->mapFromSource(playlistIndex);
    QModelIndex playlistProxyIndex = playlistProxyModel()->mapFromSource(sortProxyIndex);
    QModelIndex filterProxyIndex = mapFromSource(playlistProxyIndex);
    return filterProxyIndex;
}

QModelIndex PlaylistFilterProxyModel::mapToPlaylistModel(uint row) const
{
    QModelIndex filterProxyIndex = index(row, 0);
    QModelIndex playlistProxyIndex = mapToSource(filterProxyIndex);
    QModelIndex sortProxyIndex = playlistProxyModel()->mapToSource(playlistProxyIndex);
    QModelIndex playlistIndex = playlistSortProxyModel()->mapToSource(sortProxyIndex);
    return playlistIndex;
}

void PlaylistFilterProxyModel::splitItemSelection(const QModelIndexList &original,
                                                  int splitRow,
                                                  bool isTopDown,
                                                  QModelIndexList &lowerPart,
                                                  QModelIndexList &upperPart)
{
    for (const QModelIndex &index : original) {
        if (!index.isValid()) {
            continue;
        }
        if (index.row() < splitRow) {
            lowerPart.append(index);
        } else if (index.row() > splitRow) {
            upperPart.append(index);
        } else {
            if (isTopDown) {
                lowerPart.append(index);
            } else {
                upperPart.append(index);
            }
        }
    }
}

QModelIndexList PlaylistFilterProxyModel::selectedRows() const
{
    // QItemSelectionModel::selectedRows() does not guarantee a sorted array, which we mostly need
    QModelIndexList selection = m_selectionModel.selectedRows();
    std::sort(selection.begin(), selection.end(), [](const QModelIndex &a, const QModelIndex &b) {
        return a.row() < b.row();
    });
    return selection;
}

void PlaylistFilterProxyModel::saveInternalPlaylist(const QString &path, const QString &playlistName)
{
    // path must be the config folder
    QUrl url(path);

    QFile configPath(url.toString(QUrl::PreferLocalFile));
    QFileInfo fileInfo(configPath);

    // Check if the directory exists and create it if it does not
    QDir dir = fileInfo.dir();
    if (!dir.exists()) {
        if (!dir.mkpath(dir.absolutePath())) {
            qWarning() << "Failed to create playlist directory:" << dir.path();
            return;
        }
    }
    saveM3uFile(url.toString(QUrl::PreferLocalFile) + playlistName + u".m3u");
}

#include "moc_playlistfilterproxymodel.cpp"
