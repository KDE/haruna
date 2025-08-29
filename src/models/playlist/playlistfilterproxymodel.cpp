/*
 * SPDX-FileCopyrightText: 2025 George Florea Bănuș <georgefb899@gmail.com>
 * SPDX-FileCopyrightText: 2025 Muhammet Sadık Uğursoy <sadikugursoy@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "playlistfilterproxymodel.h"

#include <QClipboard>
#include <QDir>
#include <QFile>
#include <QGuiApplication>
#include <QMap>

#include <KFileItem>
#include <KFileMetaData/Properties>
#include <KIO/DeleteOrTrashJob>
#include <KIO/OpenFileManagerWindowJob>
#include <KIO/RenameFileDialog>

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

    connect(&m_selectionModel, &QItemSelectionModel::selectionChanged, this, &PlaylistFilterProxyModel::onSelectionChanged);
}

QVariant PlaylistFilterProxyModel::data(const QModelIndex &index, int role) const
{
    if (role == PlaylistModel::IsSelectedRole) {
        return QVariant(m_selectionModel.isSelected(index));
    }
    return playlistModel()->data(mapToPlaylistModel(index.row()), role);
}

uint PlaylistFilterProxyModel::selectionCount()
{
    return m_selectionModel.selectedRows().count();
}

uint PlaylistFilterProxyModel::itemCount()
{
    return rowCount();
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
    if (model->isShuffleOn()) {
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
    if (model->isShuffleOn()) {
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
    for (int i{0}; i < rowCount(); ++i) {
        QString itemPath = data(index(i, 0), PlaylistModel::PathRole).toString();
        m3uFile.write(itemPath.toUtf8().append("\n"));
    }
    m3uFile.close();
}

void PlaylistFilterProxyModel::highlightInFileManager(uint row)
{
    QString path = data(index(row, 0), PlaylistModel::PathRole).toString();
    KIO::highlightInFileManager({QUrl::fromUserInput(path)});
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

    int rowOffset = 0;
    for (const auto &iTopSelection : std::as_const(topSelection)) {
        // These indices are located above the destination. Each move will reduce index of the
        // following rows by 1 until the destination row since they are sorted in ascending order.
        // We take this into account by using rowOffset
        int first = iTopSelection.row();
        int destination = topDownDrag ? destinationRow : destinationRow - 1;

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
        rowOffset++;
    }
    rowOffset = 0;
    for (const auto &iBottomSelection : std::as_const(bottomSelection)) {
        // Indices here will be located below the destination. Moving the items to the top will not change
        // their indices, but it will change the target index. We take this into account in the same way.
        int first = iBottomSelection.row();
        int destination = topDownDrag ? destinationRow + 1 : destinationRow;

        if (beginMoveRows(QModelIndex(), first, first, QModelIndex(), destination + rowOffset)) {
            int sourceFirst = mapToSource(index(first, 0)).row();
            int sourceDestination = mapToSource(index(destination, 0)).row();
            playlistProxyModel()->moveRows(QModelIndex(), sourceFirst, 1, QModelIndex(), sourceDestination + rowOffset);
            endMoveRows();
        }
        rowOffset++;
    }
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

void PlaylistFilterProxyModel::sortItems(PlaylistSortProxyModel::Sort sortMode)
{
    playlistSortProxyModel()->sortItems(sortMode);
    Q_EMIT itemsSorted();
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
