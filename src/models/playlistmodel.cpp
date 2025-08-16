/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "playlistmodel.h"

#include <QClipboard>
#include <QCollator>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QGuiApplication>
#include <QJsonArray>

#include <KFileItem>
#include <KFileMetaData/Properties>
#include <KIO/DeleteOrTrashJob>
#include <KIO/OpenFileManagerWindowJob>
#include <KIO/RenameFileDialog>

#include <random>

#include "application.h"
#include "generalsettings.h"
#include "playlistsettings.h"
#include "worker.h"
#include "youtube.h"

using namespace Qt::StringLiterals;

PlaylistModel::PlaylistModel(QObject *parent)
    : QAbstractListModel(parent)
{
    if (PlaylistSettings::playbackBehavior() == u"Random"_s) {
        m_isShuffleOn = true;
        shuffleIndexes();
    }

    connect(PlaylistSettings::self(), &PlaylistSettings::PlaybackBehaviorChanged, this, [this]() {
        if (PlaylistSettings::playbackBehavior() == u"Random"_s) {
            m_isShuffleOn = true;
            shuffleIndexes();
        } else {
            m_isShuffleOn = false;
        }
    });
    connect(this, &PlaylistModel::itemAdded, Worker::instance(), &Worker::getMetaData);
    connect(&youtube, &YouTube::playlistRetrieved, this, &PlaylistModel::addYouTubePlaylist);
    connect(&youtube, &YouTube::videoInfoRetrieved, this, &PlaylistModel::updateFileInfo);

    connect(Worker::instance(), &Worker::metaDataReady, this, [=](int i, const QUrl &url, KFileMetaData::PropertyMultiMap metaData) {
        if (m_playlist.empty() || i > m_playlist.size()) {
            return;
        }
        if (m_playlist[i].url == url) {
            auto duration = metaData.value(KFileMetaData::Property::Duration).toInt();
            auto title = metaData.value(KFileMetaData::Property::Title).toString();

            m_playlist[i].formattedDuration = Application::formatTime(duration);
            m_playlist[i].duration = duration;
            m_playlist[i].mediaTitle = title;

            Q_EMIT dataChanged(index(i, 0), index(i, 0));
        } else {
            qDebug() << "\n"
                     << u"Data missmatch: the url at position %1 received from the worker thread:"_s.arg(i) << "\n"
                     << u"%1"_s.arg(url.toString()) << "\n"
                     << u"is different than the url in m_playlist at position %2"_s.arg(i) << "\n"
                     << u"%1"_s.arg(m_playlist[i].url.toString());
        }
    });
}

int PlaylistModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_playlist.size();
}

QVariant PlaylistModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    auto item = m_playlist.at(index.row());
    switch (role) {
    case NameRole:
        return QVariant(item.filename);
    case TitleRole:
        return item.mediaTitle.isEmpty() ? QVariant(item.filename) : QVariant(item.mediaTitle);
    case PathRole:
        return QVariant(item.url);
    case DurationRole:
        return QVariant(item.formattedDuration);
    case PlayingRole:
        return QVariant(static_cast<int>(m_playingItem) == index.row());
    case FolderPathRole:
        return QVariant(item.folderPath);
    case IsLocalRole:
        return QVariant(!item.url.scheme().startsWith(u"http"_s));
    }

    return QVariant();
}

QHash<int, QByteArray> PlaylistModel::roleNames() const
{
    // clang-format off
    QHash<int, QByteArray> roles = {
        {NameRole,       QByteArrayLiteral("name")},
        {TitleRole,      QByteArrayLiteral("title")},
        {PathRole,       QByteArrayLiteral("path")},
        {FolderPathRole, QByteArrayLiteral("folderPath")},
        {DurationRole,   QByteArrayLiteral("duration")},
        {PlayingRole,    QByteArrayLiteral("isPlaying")},
        {IsLocalRole,    QByteArrayLiteral("isLocal")},
    };
    // clang-format on

    return roles;
}

void PlaylistModel::clear()
{
    m_playlistPath = QString();
    m_playingItem = -1;
    beginResetModel();
    m_playlist.clear();
    endResetModel();
}

void PlaylistModel::addItem(const QString &path, Behavior behavior)
{
    auto url = QUrl::fromUserInput(path);
    addItem(url, behavior);
}

void PlaylistModel::addItem(const QUrl &url, Behavior behavior)
{
    if (!url.isValid() || url.isEmpty()) {
        return;
    }
    if (behavior == Behavior::Clear) {
        clear();
    }

    if (url.scheme() == u"file"_s) {
        auto mimeType = Application::mimeType(url);

        if (mimeType == u"audio/x-mpegurl"_s) {
            m_playlistPath = url.toString();
            addM3uItems(url, behavior);
            return;
        }

        if (isVideoOrAudioMimeType(mimeType)) {
            if (behavior == Behavior::Clear) {
                if (PlaylistSettings::loadSiblings()) {
                    getSiblingItems(url);
                } else {
                    appendItem(url);
                    setPlayingItem(0);
                }
            }
            if (behavior == Behavior::Append) {
                appendItem(url);
            }
            if (behavior == Behavior::AppendAndPlay) {
                appendItem(url);
                setPlayingItem(m_playlist.size() - 1);
            }

            return;
        }
    }

    if (url.scheme() == u"http"_s || url.scheme() == u"https"_s) {
        if (youtube.isPlaylist(url)) {
            youtube.getPlaylist(url);
        } else {
            if (behavior == Behavior::Clear) {
                appendItem(url);
                setPlayingItem(0);
            }
            if (behavior == Behavior::Append) {
                appendItem(url);
            }
            if (behavior == Behavior::AppendAndPlay) {
                appendItem(url);
                setPlayingItem(m_playlist.size() - 1);
            }
        }
    }
}

void PlaylistModel::addItems(const QList<QUrl> &urls, Behavior behavior)
{
    for (const auto &url : urls) {
        addItem(url, behavior);
    }
}
void PlaylistModel::appendItem(const QUrl &url)
{
    PlaylistItem item;
    QFileInfo itemInfo(url.toLocalFile());
    auto row{m_playlist.size()};
    if (itemInfo.exists() && itemInfo.isFile()) {
        item.url = url;
        item.filename = itemInfo.fileName();
        item.folderPath = itemInfo.absolutePath();
    } else {
        if (url.scheme().startsWith(u"http"_s)) {
            item.url = url;
            item.filename = url.toString();
            // causes issues with lots of links
            if (m_httpItemCounter < 20) {
                QVariantMap data{{u"row"_s, QVariant::fromValue(row)}};
                youtube.getVideoInfo(url, data);
                ++m_httpItemCounter;
            }
        }
    }

    if (item.url.isEmpty()) {
        return;
    }

    beginInsertRows(QModelIndex(), m_playlist.size(), m_playlist.size());

    m_playlist.push_back(item);
    Q_EMIT itemAdded(row, item.url.toString());

    endInsertRows();

    if (m_isShuffleOn) {
        shuffleIndexes();
    }
}

void PlaylistModel::getSiblingItems(const QUrl &url)
{
    QFileInfo openedFileInfo(url.toLocalFile());
    if (!openedFileInfo.exists() || !openedFileInfo.isFile()) {
        return;
    }

    QStringList siblingFiles;
    QDirIterator it(openedFileInfo.absolutePath(), QDir::Files | QDir::Hidden, QDirIterator::NoIteratorFlags);
    while (it.hasNext()) {
        QString siblingFile = it.next();
        QFileInfo siblingFileInfo(siblingFile);
        auto siblingUrl = QUrl::fromLocalFile(siblingFile);
        QString mimeType = Application::mimeType(siblingUrl);
        if (!siblingFileInfo.exists()) {
            continue;
        }
        if (isVideoOrAudioMimeType(mimeType)) {
            siblingFiles.append(siblingFileInfo.absoluteFilePath());
        }
    }

    QCollator collator;
    collator.setNumericMode(true);
    std::sort(siblingFiles.begin(), siblingFiles.end(), collator);

    uint playingItem{0};
    beginInsertRows(QModelIndex(), 0, siblingFiles.count() - 1);
    for (const auto &file : siblingFiles) {
        QFileInfo fileInfo(file);
        auto fileUrl = QUrl::fromLocalFile(file);
        PlaylistItem item;
        item.url = fileUrl;
        item.filename = fileInfo.fileName();
        item.folderPath = fileInfo.absolutePath();
        m_playlist.push_back(item);
        // in flatpak the file dialog gives a percent encoded path
        // use toLocalFile to normalize the urls
        if (url.toLocalFile() == fileUrl.toLocalFile()) {
            playingItem = m_playlist.size() - 1;
        }
        Q_EMIT itemAdded(m_playlist.size() - 1, item.url.toString());
    }
    endInsertRows();

    setPlayingItem(playingItem);

    if (m_isShuffleOn) {
        shuffleIndexes();
    }
}

void PlaylistModel::addM3uItems(const QUrl &url, Behavior behavior)
{
    if (url.scheme() != u"file"_s || Application::mimeType(url) != u"audio/x-mpegurl"_s) {
        return;
    }

    QFile m3uFile(url.toString(QUrl::PreferLocalFile));
    if (!m3uFile.open(QFile::ReadOnly)) {
        qDebug() << "can't open playlist file";
        return;
    }

    uint playingItem{0};
    int i{0};
    bool matchFound{false};
    while (!m3uFile.atEnd()) {
        QByteArray line = QByteArray::fromPercentEncoding(m3uFile.readLine().simplified());
        // ignore comments
        if (line.startsWith("#")) {
            continue;
        }

        // always set the working directory
        // it doesn't affect absolute paths and it's required for relative paths
        auto url = QUrl::fromUserInput(QString::fromUtf8(line), QFileInfo(m3uFile).absolutePath());
        addItem(url, Behavior::Append);

        if (!matchFound && url == QUrl::fromUserInput(GeneralSettings::lastPlayedFile())) {
            playingItem = i;
            matchFound = true;
        }
        ++i;
    }
    m3uFile.close();

    if (behavior == Behavior::Clear) {
        setPlayingItem(playingItem);
    }

    if (m_isShuffleOn) {
        shuffleIndexes();
    }
}

void PlaylistModel::addYouTubePlaylist(QJsonArray playlist, const QString &videoId, const QString &playlistId)
{
    bool matchFound{false};
    for (int i = 0; i < playlist.size(); ++i) {
        auto id = playlist[i][u"id"_s].toString();
        auto url = u"https://www.youtube.com/watch?v=%1&list=%2"_s.arg(id, playlistId);
        auto title = playlist[i][u"title"_s].toString();
        auto duration = playlist[i][u"duration"_s].toDouble();

        PlaylistItem item;
        item.url = QUrl::fromUserInput(url);
        item.filename = !title.isEmpty() ? title : url;
        item.mediaTitle = !title.isEmpty() ? title : url;
        item.formattedDuration = Application::formatTime(duration);
        item.duration = duration;

        beginInsertRows(QModelIndex(), m_playlist.size(), m_playlist.size());
        m_playlist.push_back(item);
        Q_EMIT itemAdded(i, item.url.toString());
        endInsertRows();

        if (videoId.isEmpty()) {
            // when videoId is not available check if GeneralSettings::lastPlayedFile()
            // is part of the playlist and set the matching item as playing
            if (GeneralSettings::lastPlayedFile().contains(id) && !matchFound) {
                setPlayingItem(i);
                matchFound = true;
            }
        } else {
            // when videoId is available set the item with the same id as playing
            if (videoId == id && !matchFound) {
                setPlayingItem(i);
                matchFound = true;
            }
        }
    }

    if (!matchFound) {
        setPlayingItem(0);
    }
}

void PlaylistModel::updateFileInfo(YTVideoInfo info, QVariantMap data)
{
    const auto row = data.value(u"row"_s).toUInt();
    const auto item{m_playlist[row]};
    if (info.url != item.url) {
        return;
    }

    m_playlist[row].mediaTitle = info.mediaTitle;
    m_playlist[row].filename = info.mediaTitle;
    m_playlist[row].formattedDuration = Application::formatTime(info.duration);
    m_playlist[row].duration = info.duration;

    --m_httpItemCounter;

    Q_EMIT dataChanged(index(row, 0), index(row, 0));
}

bool PlaylistModel::isVideoOrAudioMimeType(const QString &mimeType)
{
    // clang-format off
    return (mimeType.startsWith(u"video/"_s)
            || mimeType.startsWith(u"audio/"_s)
            || mimeType == u"application/vnd.rn-realmedia"_s)
            && mimeType != u"audio/x-mpegurl"_s;
    // clang-format on
}

void PlaylistModel::setPlayingItem(uint i)
{
    if (i >= m_playlist.size()) {
        return;
    }

    uint previousItem{m_playingItem};
    m_playingItem = i;
    Q_EMIT dataChanged(index(previousItem, 0), index(previousItem, 0));
    Q_EMIT dataChanged(index(i, 0), index(i, 0));

    Q_EMIT playingItemChanged();

    GeneralSettings::setLastPlayedFile(m_playlist[i].url.toString());
    GeneralSettings::setLastPlaylist(m_playlistPath);
    GeneralSettings::self()->save();
}

bool PlaylistModel::isShuffleOn() const
{
    return m_isShuffleOn;
}

void PlaylistModel::shuffleIndexes()
{
    if (m_playlist.size() <= 0) {
        return;
    }
    m_currentShuffledIndex = 0;
    m_shuffledIndexes.resize(m_playlist.size());
    std::iota(m_shuffledIndexes.begin(), m_shuffledIndexes.end(), 0);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(m_shuffledIndexes.begin(), m_shuffledIndexes.end(), gen);

    // Move current item to start
    auto it = std::find(m_shuffledIndexes.begin(), m_shuffledIndexes.end(), m_playingItem);
    if (it != m_shuffledIndexes.end()) {
        std::rotate(m_shuffledIndexes.begin(), it, it + 1);
    }
}

std::vector<int> PlaylistModel::shuffledIndexes() const
{
    return m_shuffledIndexes;
}

int PlaylistModel::currentShuffledIndex() const
{
    return m_currentShuffledIndex;
}

void PlaylistModel::setCurrentShuffledIndex(int shuffledIndex)
{
    m_currentShuffledIndex = shuffledIndex;
}

PlaylistProxyModel::PlaylistProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setDynamicSortFilter(true);
}

void PlaylistProxyModel::sortItems(Sort sortMode)
{
    switch (sortMode) {
    case Sort::NameAscending: {
        setSortRole(PlaylistModel::NameRole);
        sort(0, Qt::AscendingOrder);
        break;
    }
    case Sort::NameDescending: {
        setSortRole(PlaylistModel::NameRole);
        sort(0, Qt::DescendingOrder);
        break;
    }
    case Sort::DurationAscending: {
        setSortRole(PlaylistModel::DurationRole);
        sort(0, Qt::AscendingOrder);
        break;
    }
    case Sort::DurationDescending: {
        setSortRole(PlaylistModel::DurationRole);
        sort(0, Qt::DescendingOrder);
        break;
    }
    }
}

uint PlaylistProxyModel::getPlayingItem()
{
    auto model = qobject_cast<PlaylistModel *>(sourceModel());
    return mapFromSource(model->index(model->m_playingItem, 0)).row();
}

void PlaylistProxyModel::setPlayingItem(uint i)
{
    auto model = qobject_cast<PlaylistModel *>(sourceModel());
    model->setPlayingItem(mapToSource(index(i, 0)).row());
}

void PlaylistProxyModel::playNext()
{
    auto model = qobject_cast<PlaylistModel *>(sourceModel());
    if (model->isShuffleOn()) {
        auto nextIndex = model->currentShuffledIndex() + 1;
        if (nextIndex >= static_cast<int>(model->shuffledIndexes().size())) {
            nextIndex = 0;
        }
        auto shuffledIndex = model->shuffledIndexes().at(nextIndex);
        model->setCurrentShuffledIndex(nextIndex);
        model->setPlayingItem(shuffledIndex);
    } else {
        auto currentIndex = mapFromSource(model->index(model->m_playingItem, 0)).row();
        auto nextIndex = currentIndex + 1;
        if (nextIndex < rowCount()) {
            setPlayingItem(nextIndex);
        } else {
            setPlayingItem(0);
        }
    }
}

void PlaylistProxyModel::playPrevious()
{
    auto model = qobject_cast<PlaylistModel *>(sourceModel());
    if (model->isShuffleOn()) {
        auto previousIndex = model->currentShuffledIndex() - 1;
        if (previousIndex < 0) {
            previousIndex = model->shuffledIndexes().size() - 1;
        }
        auto shuffledIndex = model->shuffledIndexes().at(previousIndex);
        model->setCurrentShuffledIndex(previousIndex);
        model->setPlayingItem(shuffledIndex);
    } else {
        auto currentIndex = mapFromSource(model->index(model->m_playingItem, 0)).row();
        auto previousIndex = currentIndex - 1;

        if (previousIndex >= 0) {
            setPlayingItem(index(previousIndex, 0).row());
        }
    }
}

void PlaylistProxyModel::saveM3uFile(const QString &path)
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

void PlaylistProxyModel::highlightInFileManager(uint row)
{
    QString path = data(index(row, 0), PlaylistModel::PathRole).toString();
    KIO::highlightInFileManager({QUrl::fromUserInput(path)});
}

void PlaylistProxyModel::removeItem(uint row)
{
    auto model = qobject_cast<PlaylistModel *>(sourceModel());

    auto sourceRow = mapFromSource(model->index(row)).row();

    beginRemoveRows(QModelIndex(), sourceRow, sourceRow);
    model->m_playlist.erase(model->m_playlist.begin() + sourceRow);
    endRemoveRows();
}

void PlaylistProxyModel::renameFile(uint row)
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
        auto model = qobject_cast<PlaylistModel *>(sourceModel());
        auto sourceRow = mapToSource(index(row, 0)).row();
        auto item = model->m_playlist.at(sourceRow);
        item.url = QUrl::fromUserInput(urls.first().path());
        item.filename = urls.first().fileName();

        Q_EMIT dataChanged(index(row, 0), index(row, 0));
    });
}

void PlaylistProxyModel::trashFile(uint row)
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
            auto model = qobject_cast<PlaylistModel *>(sourceModel());
            auto sourceRow = mapToSource(index(row, 0)).row();
            beginRemoveRows(QModelIndex(), sourceRow, sourceRow);
            model->m_playlist.erase(model->m_playlist.begin() + sourceRow);
            endRemoveRows();
        }
    });
}

void PlaylistProxyModel::copyFileName(uint row)
{
    auto model = qobject_cast<PlaylistModel *>(sourceModel());
    auto item = model->m_playlist.at(row);
    QGuiApplication::clipboard()->setText(item.filename);
}

void PlaylistProxyModel::copyFilePath(uint row)
{
    auto model = qobject_cast<PlaylistModel *>(sourceModel());
    auto item = model->m_playlist.at(row);
    QGuiApplication::clipboard()->setText(item.url.toString());
}

QString PlaylistProxyModel::getFilePath(uint row)
{
    auto model = qobject_cast<PlaylistModel *>(sourceModel());
    auto item = model->m_playlist.at(row);
    return item.url.toString();
}

bool PlaylistProxyModel::isLastItem(uint row)
{
    return static_cast<int>(row) == rowCount() - 1;
}

#include "moc_playlistmodel.cpp"
