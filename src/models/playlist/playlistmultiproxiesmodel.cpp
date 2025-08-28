/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "playlistmultiproxiesmodel.h"

#include <QDir>
#include <QLineEdit>
#include <QStandardPaths>

#include <KFileItem>
#include <KIO/RenameFileDialog>
#include <KLocalizedString>

#include "application.h"
#include "playlistrenamevalidator.h"

using namespace Qt::StringLiterals;

PlaylistMultiProxiesModel::PlaylistMultiProxiesModel(QObject *parent)
    : QAbstractListModel{parent}
{
    addPlaylist(QString(u"Default"_s));

    QUrl cacheUrl = getPlaylistCacheUrl();
    if (cacheUrl.isEmpty()) {
        return;
    }

    QFile cacheFile(cacheUrl.toString(QUrl::PreferLocalFile));
    if (!cacheFile.open(QFile::ReadOnly)) {
        qDebug() << "Can't open internal playlist cache";
        return;
    }

    while (!cacheFile.atEnd()) {
        QByteArray line = QByteArray::fromPercentEncoding(cacheFile.readLine().simplified());
        QString playlistName = QString::fromUtf8(line);
        if (playlistName == u"Default"_s) {
            continue;
        }
        QUrl playlistUrl = getPlaylistUrl(playlistName);
        if (playlistUrl.isEmpty()) {
            continue;
        }
        addPlaylist(playlistName, playlistUrl);
    }
    cacheFile.close();
    savePlaylistCache();
}

uint PlaylistMultiProxiesModel::activeIndex()
{
    return m_activeIndex;
}

void PlaylistMultiProxiesModel::setActiveIndex(uint pIndex)
{
    uint prev = m_activeIndex;
    m_activeIndex = pIndex;

    Q_EMIT dataChanged(index(prev, 0), index(prev, 0));
    Q_EMIT dataChanged(index(m_activeIndex, 0), index(m_activeIndex, 0));

    Q_EMIT activeIndexChanged();
}

uint PlaylistMultiProxiesModel::visibleIndex()
{
    return m_visibleIndex;
}

void PlaylistMultiProxiesModel::setVisibleIndex(uint pIndex)
{
    uint prev = m_visibleIndex;
    m_visibleIndex = pIndex;

    Q_EMIT dataChanged(index(prev, 0), index(prev, 0));
    Q_EMIT dataChanged(index(m_visibleIndex, 0), index(m_visibleIndex, 0));

    Q_EMIT visibleIndexChanged();
}

int PlaylistMultiProxiesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_playlistFilterProxyModels.size();
}

QVariant PlaylistMultiProxiesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    switch (role) {
    case NameRole:
        return QVariant(m_playlistFilterProxyModels[index.row()]->playlistModel()->m_playlistName);
    case VisibleRole:
        return QVariant(static_cast<int>(m_visibleIndex) == index.row());
    case ActiveRole:
        return QVariant(static_cast<int>(m_activeIndex) == index.row());
    }

    return QVariant();
}

QHash<int, QByteArray> PlaylistMultiProxiesModel::roleNames() const
{
    // clang-format off
    QHash<int, QByteArray> roles = {
    {NameRole,     QByteArrayLiteral("name")},
    {VisibleRole, QByteArrayLiteral("isVisible")},
    {ActiveRole,   QByteArrayLiteral("isActive")},
    };
    // clang-format on

    return roles;
}

void PlaylistMultiProxiesModel::addPlaylist(QString playlistName, QUrl internalUrl)
{
    uint playlistsSize = m_playlistFilterProxyModels.size();
    for (uint i = 0; i < playlistsSize; ++i) {
        QString pName = m_playlistFilterProxyModels[i]->playlistModel()->m_playlistName;
        if (playlistName == pName) {
            Q_EMIT Application::instance()->error(
                i18nc("@info:tooltip; %1 playlist with same name", "Playlists with same name is not allowed: %1", playlistName));
            return;
        }
    }

    auto filterModel = std::make_unique<PlaylistFilterProxyModel>();
    filterModel->playlistModel()->m_playlistName = playlistName;

    if (!internalUrl.isEmpty()) {
        filterModel->playlistModel()->addM3uItems(internalUrl, PlaylistModel::Behavior::Clear);
    }

    connect(filterModel->playlistModel(), &PlaylistModel::playingItemChanged, this, [=](QString pName) {
        // When playingItemChanged is emitted, we check if the new playing item is in the currently active
        // playlist. If not, we stop that playlist and update the active one.
        QString activePlaylistName = m_playlistFilterProxyModels[m_activeIndex]->playlistModel()->m_playlistName;
        if (activePlaylistName != pName) {
            // Double clicked on an item in a different list
            activeFilterProxy()->playlistModel()->stop();
            setActiveIndex(m_visibleIndex);
        }
        Q_EMIT playingItemChanged();
    });

    // When underlying models change, either by remove, insert, move or sort: save the playlist
    connect(filterModel.get(), &PlaylistFilterProxyModel::itemsSorted, this, &PlaylistMultiProxiesModel::saveVisiblePlaylist);
    connect(filterModel.get(), &PlaylistFilterProxyModel::itemsMoved, this, &PlaylistMultiProxiesModel::saveVisiblePlaylist);
    connect(filterModel.get(), &PlaylistFilterProxyModel::itemsRemoved, this, &PlaylistMultiProxiesModel::saveVisiblePlaylist);
    connect(filterModel.get(), &PlaylistFilterProxyModel::itemsInserted, this, &PlaylistMultiProxiesModel::saveVisiblePlaylist);

    filterModel->playlistModel()->stop();

    beginInsertRows(QModelIndex(), playlistsSize, playlistsSize);
    m_playlistFilterProxyModels.push_back(std::move(filterModel));
    endInsertRows();
}

void PlaylistMultiProxiesModel::removePlaylist(uint pIndex)
{
    // Cannot and should not delete default
    QString playlistName = m_playlistFilterProxyModels[pIndex]->playlistModel()->m_playlistName;
    if (playlistName == u"Default"_s) {
        return;
    }
    // Removing the active (currently playing) tab
    if (pIndex == m_activeIndex) {
        activeFilterProxy()->playlistModel()->stop();
        m_activeIndex = pIndex - 1;
        Q_EMIT activeIndexChanged();
    }
    // Removing the visible tab, signal the views
    if (pIndex == m_visibleIndex) {
        m_visibleIndex = m_activeIndex;
        Q_EMIT dataChanged(index(m_visibleIndex, 0), index(m_visibleIndex, 0));
        Q_EMIT visibleIndexChanged();
    }
    if (pIndex < m_visibleIndex) {
        m_visibleIndex -= 1;
        Q_EMIT visibleIndexChanged();
    }
    if (pIndex < m_activeIndex) {
        m_activeIndex -= 1;
    }

    // Remove the deleted playlist
    QUrl playlistUrl = getPlaylistUrl(playlistName);
    if (!playlistUrl.isEmpty()) {
        QFile playlistFile(playlistUrl.toString(QUrl::PreferLocalFile));
        playlistFile.remove();
    }

    beginRemoveRows(QModelIndex(), pIndex, pIndex);
    m_playlistFilterProxyModels.erase(m_playlistFilterProxyModels.cbegin() + pIndex);
    endRemoveRows();
    savePlaylistCache();
}

void PlaylistMultiProxiesModel::movePlaylist(uint row, uint destinationRow)
{
    // Prevent an invalid move
    if (destinationRow == row) {
        return;
    }
    if (row == 0 || destinationRow == 0) {
        return;
    }
    if (row > m_playlistFilterProxyModels.size() || destinationRow > m_playlistFilterProxyModels.size()) {
        return;
    }

    QString activePlaylistName = m_playlistFilterProxyModels[m_activeIndex]->playlistModel()->m_playlistName;

    auto bFProxy = m_playlistFilterProxyModels.begin();
    bool leftDrag = destinationRow < row;
    if (leftDrag) {
        if (beginMoveRows(QModelIndex(), row, row, QModelIndex(), destinationRow)) {
            std::rotate(bFProxy + destinationRow, bFProxy + row, bFProxy + row + 1);
        }
        endMoveRows();
        Q_EMIT dataChanged(index(destinationRow, 0), index(row, 0));
    } else {
        if (beginMoveRows(QModelIndex(), row, row, QModelIndex(), destinationRow + 1)) {
            std::rotate(bFProxy + row, bFProxy + row + 1, bFProxy + destinationRow + 1);
        }
        endMoveRows();
        Q_EMIT dataChanged(index(row, 0), index(destinationRow, 0));
    }

    uint playlistsSize = m_playlistFilterProxyModels.size();
    for (uint i = 0; i < playlistsSize; ++i) {
        QString pName = m_playlistFilterProxyModels[i]->playlistModel()->m_playlistName;
        if (activePlaylistName == pName) {
            uint prev = m_activeIndex;
            m_activeIndex = i;
            Q_EMIT dataChanged(index(prev, 0), index(prev, 0));
            Q_EMIT dataChanged(index(m_activeIndex, 0), index(m_activeIndex, 0));
            Q_EMIT activeIndexChanged();
        }
        // No need to check visible playlist index changes, TabBar handles it
    }

    savePlaylistCache();
    Q_EMIT visibleIndexChanged();
}

void PlaylistMultiProxiesModel::renamePlaylist(uint pIndex)
{
    if (pIndex == 0) {
        return;
    }

    QString tabName = m_playlistFilterProxyModels[pIndex]->playlistModel()->m_playlistName;
    auto configPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation).append(u"/haruna/playlists/"_s);
    QUrl url(configPath + tabName + u".m3u"_s);
    if (url.scheme().isEmpty()) {
        url.setScheme(u"file"_s);
    }
    KFileItem item(url);
    auto renameDialog = new KIO::RenameFileDialog(KFileItemList({item}), nullptr);
    // Hack into line edit to override erasing '.m3u' extension
    QLineEdit *edit = renameDialog->findChild<QLineEdit *>();
    if (edit) {
        edit->setValidator(new PlaylistRenameValidator());
    }
    renameDialog->open();

    connect(renameDialog, &KIO::RenameFileDialog::renamingFinished, this, [=](const QList<QUrl> &urls) {
        QString inputText = urls.first().fileName();
        QString playlistName = inputText.first(inputText.length() - 4); // '.m3u' 4 chars

        m_playlistFilterProxyModels[pIndex]->playlistModel()->m_playlistName = playlistName;
        savePlaylistCache();
        Q_EMIT dataChanged(index(pIndex, 0), index(pIndex, 0));
    });
}

void PlaylistMultiProxiesModel::resetTabView()
{
    Q_EMIT layoutChanged();
}

PlaylistFilterProxyModel *PlaylistMultiProxiesModel::activeFilterProxy()
{
    return m_playlistFilterProxyModels[m_activeIndex].get();
}

PlaylistFilterProxyModel *PlaylistMultiProxiesModel::visibleFilterProxy()
{
    return m_playlistFilterProxyModels[m_visibleIndex].get();
}

PlaylistFilterProxyModel *PlaylistMultiProxiesModel::defaultFilterProxy()
{
    return m_playlistFilterProxyModels[0].get();
}

QUrl PlaylistMultiProxiesModel::getPlaylistCacheUrl()
{
    auto configPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    auto filePath = configPath.append(u"/haruna/playlist.cache"_s);
    QUrl url = QUrl::fromLocalFile(filePath);
    QFile cacheFile(url.toString(QUrl::PreferLocalFile));

    if (!cacheFile.exists()) {
        // Create the cache if it does not exist
        if (!cacheFile.open(QFile::WriteOnly)) {
            qWarning() << "Failed to create playlist cache";
            return QUrl();
        }
        cacheFile.write(QString(u"Default").toUtf8());
        cacheFile.close();
    }
    return url;
}

QUrl PlaylistMultiProxiesModel::getPlaylistUrl(QString playlistName)
{
    auto configPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    auto playlistsPath = configPath.append(u"/haruna/playlists/"_s);
    auto filePath = playlistsPath.append(playlistName).append(u".m3u"_s);

    QUrl url = QUrl::fromLocalFile(filePath);
    QFile playlistFile(url.toString(QUrl::PreferLocalFile));

    if (!playlistFile.exists()) {
        return QUrl();
    }
    return url;
}

void PlaylistMultiProxiesModel::saveVisiblePlaylist()
{
    // Note: this method saves unfiltered whole list.
    auto configPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation).append(u"/haruna/playlists/"_s);

    QString visiblePlaylistName = m_playlistFilterProxyModels[m_visibleIndex]->playlistModel()->m_playlistName;
    if (visiblePlaylistName == u"Default") {
        return;
    }
    visibleFilterProxy()->saveInternalPlaylist(configPath, visiblePlaylistName);

    savePlaylistCache();
    Q_EMIT dataChanged(index(m_visibleIndex, 0), index(m_visibleIndex, 0));
}

void PlaylistMultiProxiesModel::savePlaylistCache()
{
    QUrl cacheUrl = getPlaylistCacheUrl();
    if (cacheUrl.isEmpty()) {
        return;
    }

    QFile cacheFile(cacheUrl.toString(QUrl::PreferLocalFile));
    if (!cacheFile.open(QFile::WriteOnly)) {
        qDebug() << "Can't open internal playlist file";
        return;
    }

    uint playlistsSize = m_playlistFilterProxyModels.size();
    for (uint i = 0; i < playlistsSize; ++i) {
        QString playlistName = m_playlistFilterProxyModels[i]->playlistModel()->m_playlistName;
        cacheFile.write(playlistName.toUtf8().append("\n"));
    }

    cacheFile.close();
}

#include "moc_playlistmultiproxiesmodel.cpp"
