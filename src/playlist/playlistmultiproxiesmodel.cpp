/*
 * SPDX-FileCopyrightText: 2025 Muhammet Sadık Uğursoy <sadikugursoy@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "playlistmultiproxiesmodel.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QDir>
#include <QJsonDocument>
#include <QLineEdit>

#include <KFileItem>
#include <KIO/RenameFileDialog>
#include <KLocalizedString>

#include "miscutils.h"
#include "pathutils.h"
#include "playlistrenamevalidator.h"

using namespace Qt::StringLiterals;

inline void swap(QJsonValueRef v1, QJsonValueRef v2)
{
    QJsonValue temp(v1);
    v1 = QJsonValue(v2);
    v2 = temp;
}

PlaylistMultiProxiesModel::PlaylistMultiProxiesModel(QObject *parent)
    : QAbstractListModel{parent}
{
    QCommandLineParser parser;
    parser.process(*QApplication::instance());
    QList<QUrl> urls;
    const auto args = parser.positionalArguments();
    for (const auto &arg : args) {
        urls.append(QUrl::fromUserInput(arg, QDir::currentPath()));
    }

    QUrl cacheUrl = getPlaylistCacheUrl();
    if (cacheUrl.isEmpty()) {
        return;
    }

    QFile cacheFile(cacheUrl.toString(QUrl::PreferLocalFile));
    if (!cacheFile.open(QFile::ReadOnly)) {
        qDebug() << "Can't open internal playlist cache";
        return;
    }

    const QByteArray data = cacheFile.readAll();
    cacheFile.close();

    QJsonParseError parseErr;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseErr);
    if (parseErr.error != QJsonParseError::NoError) {
        qWarning() << "Failed loading playlist cache";
        savePlaylistCache();
        return;
    }

    if (doc.isArray()) {
        for (int i = 0; i < doc.array().count(); ++i) {
            const QJsonValue &value = doc.array().at(i);
            if (!value.isObject()) {
                continue;
            }
            QJsonObject playlist = value.toObject();
            bool active = playlist.value(u"isActive").toBool();

            QString playlistName = playlist.value(u"name").toString();
            QUrl playlistUrl = getPlaylistUrl(playlistName);
            if (playlistName == u"Default"_s) {
                playlistUrl = getDefaultPlaylistUrl();
            }
            if (playlistUrl.isEmpty()) {
                continue;
            }

            PlaylistsModelItem item;
            item.playlistName = playlistName;
            item.playlistUrl = playlistUrl;
            item.jsonData = playlist;

            addPlaylist(std::move(item));
            if (!urls.isEmpty()) {
                initPlaylist(0, false);
                continue;
            }
            if (active) {
                initPlaylist(i);
            }
        }
    }
}

uint PlaylistMultiProxiesModel::activeIndex()
{
    return m_activeIndex;
}

void PlaylistMultiProxiesModel::setActiveIndex(uint pIndex)
{
    if (m_activeIndex == pIndex) {
        return;
    }

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
    if (m_visibleIndex == pIndex) {
        return;
    }

    uint prev = m_visibleIndex;
    m_visibleIndex = pIndex;

    Q_EMIT dataChanged(index(prev, 0), index(prev, 0));
    Q_EMIT dataChanged(index(m_visibleIndex, 0), index(m_visibleIndex, 0));

    Q_EMIT visibleIndexChanged();
}

int PlaylistMultiProxiesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_data.size();
}

QVariant PlaylistMultiProxiesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }
    switch (role) {
    case NameRole:
        return m_data.at(index.row()).playlistName;
    case VisibleRole:
        return static_cast<int>(m_visibleIndex) == index.row();
    case ActiveRole:
        return static_cast<int>(m_activeIndex) == index.row();
    }

    return {};
}

QHash<int, QByteArray> PlaylistMultiProxiesModel::roleNames() const
{
    // clang-format off
    static QHash<int, QByteArray> roles = {
        {NameRole,    QByteArrayLiteral("name")},
        {VisibleRole, QByteArrayLiteral("isVisible")},
        {ActiveRole,  QByteArrayLiteral("isActive")},
    };
    // clang-format on

    return roles;
}

void PlaylistMultiProxiesModel::addPlaylist(PlaylistsModelItem newItem)
{
    for (const auto &item : m_data) {
        if (newItem.playlistName == item.playlistName) {
            Q_EMIT MiscUtils::instance()->error(
                i18nc("@info:tooltip; %1 playlist with same name", "Playlists with same name is not allowed: %1", newItem.playlistName));
            return;
        }
    }

    beginInsertRows(QModelIndex(), m_data.size(), m_data.size());
    m_data.push_back(std::move(newItem));
    endInsertRows();
}

void PlaylistMultiProxiesModel::initPlaylist(uint row, bool addItemsToPlaylist)
{
    auto &item = m_data[row];
    item.playlist = std::make_unique<PlaylistFilterProxyModel>();
    item.isInitialized = true;
    item.playlist->playlistModel()->setPlaylistName(item.playlistName);

    if (addItemsToPlaylist && !item.playlistUrl.isEmpty()) {
        item.playlist->playlistModel()->addM3uItems(item.playlistUrl, PlaylistModel::Behavior::Append);
    }

    connect(item.playlist->playlistModel(), &PlaylistModel::playingItemChanged, this, [this](const QString &pName) {
        // When playingItemChanged is emitted, we check if the new playing item is in the currently active
        // playlist. If not, we stop that playlist and update the active one.
        QString activePlaylistName = m_data.at(m_activeIndex).playlistName;
        if (activePlaylistName != pName) {
            // Either changed the item via GUI, or loaded as last played item internally.
            // Get the tab index by checking the name
            activeFilterProxy()->playlistModel()->stop();
            for (uint i = 0; i < m_data.size(); ++i) {
                if (m_data.at(i).playlistName == pName) {
                    setActiveIndex(i);
                    break;
                }
            }
        }
        savePlaylistCache();
        Q_EMIT playingItemChanged();
    });

    // When underlying models change, either by remove, insert, move or sort: save the playlist
    connect(item.playlist.get(), &PlaylistFilterProxyModel::itemsSorted, this, &PlaylistMultiProxiesModel::saveVisiblePlaylist);
    connect(item.playlist.get(), &PlaylistFilterProxyModel::itemsMoved, this, &PlaylistMultiProxiesModel::saveVisiblePlaylist);
    connect(item.playlist.get(), &PlaylistFilterProxyModel::itemsRemoved, this, &PlaylistMultiProxiesModel::saveVisiblePlaylist);
    connect(item.playlist.get(), &PlaylistFilterProxyModel::itemsInserted, this, &PlaylistMultiProxiesModel::saveVisiblePlaylist);
    connect(item.playlist->playlistSortProxyModel(), &PlaylistSortProxyModel::groupingChanged, this, &PlaylistMultiProxiesModel::saveVisiblePlaylist);

    item.playlist->playlistModel()->stop();

    if (item.jsonData.contains(u"showSections")) {
        bool showSections = item.jsonData.value(u"showSections").toBool();
        item.playlist->setShowSections(showSections);
    }

    auto sortJsonArray = [](const QJsonValue &v1, const QJsonValue &v2) -> bool {
        QJsonObject obj1 = v1.toObject();
        QJsonObject obj2 = v2.toObject();

        QString val1 = obj1.value(u"index").toString();
        QString val2 = obj2.value(u"index").toString();

        return val1 < val2;
    };

    if (item.jsonData.contains(u"sortBy")) {
        QJsonArray sortProperties = item.jsonData.value(u"sortBy").toArray();
        std::sort(sortProperties.begin(), sortProperties.end(), sortJsonArray);
        for (auto property : std::as_const(sortProperties)) {
            QJsonObject sortProperty = property.toObject();
            int index = sortProperty.value(u"index").toInt();
            int sort = sortProperty.value(u"sort").toInt();
            int order = sortProperty.value(u"order").toInt();
            item.playlist->addToActiveSortProperties(sort);
            item.playlist->setSortPropertySortingOrder(index, order);
        }
    }

    if (item.jsonData.contains(u"groupBy")) {
        QJsonArray groups = item.jsonData.value(u"groupBy").toArray();
        std::sort(groups.begin(), groups.end(), sortJsonArray);
        for (auto property : std::as_const(groups)) {
            QJsonObject groupProperty = property.toObject();
            int index = groupProperty.value(u"index").toInt();
            Group group = Group(groupProperty.value(u"group").toInt());
            bool hideBlank = groupProperty.value(u"hideBlank").toBool();
            item.playlist->addToActiveGroup(group);
            item.playlist->setGroupHideBlank(index, hideBlank);
        }
    }

    // this code must be run after the sorting and grouping have been setup, else when
    // the savePlaylist() function is called (triggered by PlaylistModel::playingItemChanged signal)
    // the sorting and grouping data is not available on the playlist and will be lost
    bool active = item.jsonData.value(u"isActive").toBool();
    if (active) {
        uint index = item.jsonData.value(u"currentItem").toInt();
        setActiveIndex(row);
        setVisibleIndex(row);
    }

    Q_EMIT dataChanged(index(row, 0), index(row, 0));
}

// Used by QML side. Makes sure newly added playlists are saved.
void PlaylistMultiProxiesModel::createNewPlaylist(const QString &playlistName)
{
    PlaylistsModelItem item;
    item.playlistName = playlistName;
    addPlaylist(std::move(item));
    initPlaylist(m_data.size() - 1);
    savePlaylist(playlistName, m_data.back().playlist.get());
}

void PlaylistMultiProxiesModel::removePlaylist(uint pIndex)
{
    // Cannot and should not delete default
    QString playlistName = m_data.at(pIndex).playlistName;
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
    m_data.erase(m_data.cbegin() + pIndex);
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
    if (row > m_data.size() || destinationRow > m_data.size()) {
        return;
    }

    QString activePlaylistName = m_data.at(m_activeIndex).playlistName;

    auto bFProxy = m_data.begin();
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

    uint playlistsSize = m_data.size();
    for (uint i = 0; i < playlistsSize; ++i) {
        QString pName = m_data.at(i).playlistName;
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

    QString tabName = m_data.at(pIndex).playlistName;
    auto playlistsPath = PathUtils::instance()->playlistsFolder();
    playlistsPath.append(u"/"_s);
    QUrl url(playlistsPath + tabName + u".m3u"_s);
    if (url.scheme().isEmpty()) {
        url.setScheme(u"file"_s);
    }
    KFileItem item(url);
    auto *renameDialog = new KIO::RenameFileDialog(KFileItemList({item}), nullptr);
    // Hack into line edit to override erasing '.m3u' extension
    auto *edit = renameDialog->findChild<QLineEdit *>();
    if (edit != nullptr) {
        edit->setValidator(new PlaylistRenameValidator());
    }
    renameDialog->open();

    connect(renameDialog, &KIO::RenameFileDialog::renamingFinished, this, [this, pIndex](const QList<QUrl> &urls) {
        QString inputText = urls.first().fileName();
        QString playlistName = inputText.first(inputText.length() - 4); // '.m3u' 4 chars

        m_data.at(pIndex).playlist->playlistModel()->setPlaylistName(playlistName);
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
    if (!m_data.at(m_activeIndex).isInitialized) {
        initPlaylist(m_activeIndex);
    }
    return m_data.at(m_activeIndex).playlist.get();
}

PlaylistFilterProxyModel *PlaylistMultiProxiesModel::visibleFilterProxy()
{
    if (!m_data.at(m_visibleIndex).isInitialized) {
        initPlaylist(m_visibleIndex);
    }
    return m_data.at(m_visibleIndex).playlist.get();
}

PlaylistFilterProxyModel *PlaylistMultiProxiesModel::defaultFilterProxy()
{
    if (!m_data.front().isInitialized) {
        initPlaylist(0);
    }
    return m_data.front().playlist.get();
}

PlaylistFilterProxyModel *PlaylistMultiProxiesModel::getFilterProxy(const QString &playlistName)
{
    for (const auto &playlist : m_data) {
        if (playlistName == playlist.playlistName) {
            return playlist.playlist.get();
        }
    }
    return defaultFilterProxy();
}

QUrl PlaylistMultiProxiesModel::getPlaylistCacheUrl()
{
    auto filePath = PathUtils::instance()->configFilePath(PathUtils::ConfigFile::PlaylistCache);
    QUrl url = QUrl::fromLocalFile(filePath);
    QFile cacheFile(url.toString(QUrl::PreferLocalFile));

    if (!cacheFile.exists()) {
        // Create the cache if it does not exist
        if (!cacheFile.open(QFile::WriteOnly)) {
            qWarning() << "Failed to create playlist cache";
            return {};
        }

        QJsonObject json;
        json.insert(u"name", u"Default"_s);
        json.insert(u"isActive", true);
        json.insert(u"currentItem", 0);

        QJsonDocument doc(json);

        cacheFile.write(doc.toJson(QJsonDocument::Indented));
        cacheFile.close();
    }
    return url;
}

QUrl PlaylistMultiProxiesModel::getDefaultPlaylistUrl()
{
    auto playlistsPath = PathUtils::instance()->playlistsFolder();
    playlistsPath.append(u"/"_s).append(u"Default"_s).append(u".m3u"_s);

    if (!QFile::exists(playlistsPath)) {
        createNewPlaylist(u"Default"_s);
    }
    if (!QFile::exists(playlistsPath)) {
        qDebug() << "PlaylistMultiProxiesModel::getDefaultPlaylistUrl: failed to create Default playlist file:" << playlistsPath;
    }

    return QUrl::fromLocalFile(playlistsPath);
}

QUrl PlaylistMultiProxiesModel::getPlaylistUrl(const QString &playlistName)
{
    auto playlistsPath = PathUtils::instance()->playlistsFolder();
    playlistsPath.append(u"/"_s).append(playlistName).append(u".m3u"_s);

    if (!QFile::exists(playlistsPath)) {
        return {};
    }

    return QUrl::fromLocalFile(playlistsPath);
}

void PlaylistMultiProxiesModel::saveVisiblePlaylist()
{
    QString visiblePlaylistName = m_data.at(m_visibleIndex).playlistName;
    savePlaylist(visiblePlaylistName, visibleFilterProxy());
}

void PlaylistMultiProxiesModel::savePlaylist(const QString &playlistName, PlaylistFilterProxyModel *proxyModel)
{
    // Note: this method saves unfiltered whole list.
    auto playlistsPath = PathUtils::instance()->playlistsFolder();
    playlistsPath.append(u"/"_s);

    proxyModel->saveInternalPlaylist(playlistsPath, playlistName);
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

    QJsonArray array;
    uint playlistsSize = m_data.size();
    for (uint i = 0; i < playlistsSize; ++i) {
        if (!m_data.at(i).isInitialized) {
            array.append(m_data.at(i).jsonData);
            continue;
        }
        QJsonObject json;
        json.insert(u"name", m_data.at(i).playlistName);
        json.insert(u"isActive", (m_activeIndex == i));
        json.insert(u"currentItem", static_cast<int>(m_data.at(i).playlist->playlistModel()->playingItem()));
        json.insert(u"showSections", m_data.at(i).playlist->showSections());

        // Save sorting and grouping for non-default playlists
        QJsonArray sortProperties;
        QJsonArray groups;
        int index = 0;
        const auto activeSortProperties = m_data.at(i).playlist->activeSortPropertiesModel()->properties();
        for (const auto &property : activeSortProperties) {
            QJsonObject propertyStruct;
            propertyStruct.insert(u"index", index);
            propertyStruct.insert(u"sort", property.sort);
            propertyStruct.insert(u"order", property.order);

            sortProperties.append(propertyStruct);
            index++;
        }

        index = 0;
        const auto activeGroupProperties = m_data.at(i).playlist->activeGroupModel()->properties();
        for (const auto &property : activeGroupProperties) {
            QJsonObject groupStruct;
            groupStruct.insert(u"index", index);
            groupStruct.insert(u"group", property.sort);
            groupStruct.insert(u"hideBlank", property.hideBlank);

            groups.append(groupStruct);
            index++;
        }
        json.insert(u"sortBy", sortProperties);
        json.insert(u"groupBy", groups);
        array.append(json);
    }
    QJsonDocument doc(array);
    cacheFile.write(doc.toJson(QJsonDocument::Indented));
    cacheFile.close();
}

#include "moc_playlistmultiproxiesmodel.cpp"
