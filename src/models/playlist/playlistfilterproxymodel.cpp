/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "playlistfilterproxymodel.h"

#include <QClipboard>
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
{
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

void PlaylistFilterProxyModel::sortItems(PlaylistSortProxyModel::Sort sortMode)
{
    sortFilterModel()->sortItems(sortMode);
}

void PlaylistFilterProxyModel::clear()
{
    playlistModel()->clear();
}

void PlaylistFilterProxyModel::addItem(const QString &path, PlaylistModel::Behavior behavior)
{
    auto url = QUrl::fromUserInput(path);
    playlistModel()->addItem(url, behavior);
}

void PlaylistFilterProxyModel::addItem(const QUrl &url, PlaylistModel::Behavior behavior)
{
    playlistModel()->addItem(url, behavior);
}

void PlaylistFilterProxyModel::addItems(const QList<QUrl> &urls, PlaylistModel::Behavior behavior)
{
    for (const auto &url : urls) {
        playlistModel()->addItem(url, behavior);
    }
}

PlaylistProxyModel *PlaylistFilterProxyModel::playlistProxyModel() const
{
    return qobject_cast<PlaylistProxyModel *>(sourceModel());
}

PlaylistSortProxyModel *PlaylistFilterProxyModel::sortFilterModel() const
{
    return qobject_cast<PlaylistSortProxyModel *>(playlistProxyModel()->sourceModel());
}

PlaylistModel *PlaylistFilterProxyModel::playlistModel() const
{
    return qobject_cast<PlaylistModel *>(sortFilterModel()->sourceModel());
}

QModelIndex PlaylistFilterProxyModel::mapFromPlaylistModel(uint row) const
{
    QModelIndex playlistIndex = playlistModel()->index(row, 0);
    QModelIndex sortProxyIndex = sortFilterModel()->mapFromSource(playlistIndex);
    QModelIndex playlistProxyIndex = playlistProxyModel()->mapFromSource(sortProxyIndex);
    QModelIndex filterProxyIndex = mapFromSource(playlistProxyIndex);
    return filterProxyIndex;
}

QModelIndex PlaylistFilterProxyModel::mapToPlaylistModel(uint row) const
{
    QModelIndex filterProxyIndex = index(row, 0);
    QModelIndex playlistProxyIndex = mapToSource(filterProxyIndex);
    QModelIndex sortProxyIndex = playlistProxyModel()->mapToSource(playlistProxyIndex);
    QModelIndex playlistIndex = sortFilterModel()->mapToSource(sortProxyIndex);
    return playlistIndex;
}

#include "moc_playlistfilterproxymodel.cpp"
