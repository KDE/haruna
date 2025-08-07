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
#include <QJsonDocument>
#include <QJsonValue>
#include <QProcess>
#include <QUrlQuery>

#include <KFileItem>
#include <KFileMetaData/Properties>
#include <KIO/DeleteOrTrashJob>
#include <KIO/OpenFileManagerWindowJob>
#include <KIO/RenameFileDialog>
#include <kio_version.h>

#include "application.h"
#include "generalsettings.h"
#include "playlistsettings.h"
#include "worker.h"

using namespace Qt::StringLiterals;

PlaylistModel::PlaylistModel(QObject *parent)
    : QAbstractListModel(parent)
{
    connect(this, &PlaylistModel::itemAdded, Worker::instance(), &Worker::getMetaData);

    connect(Worker::instance(), &Worker::metaDataReady, this, [=](int i, const QUrl &url, KFileMetaData::PropertyMultiMap metaData) {
        if (m_playlist.isEmpty() || i > m_playlist.size()) {
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
    return m_playlist.count();
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

void PlaylistModel::addItem(const QString &path, Behaviour behaviour)
{
    auto url = QUrl::fromUserInput(path);
    addItem(url, behaviour);
}

void PlaylistModel::addItem(const QUrl &url, Behaviour behaviour)
{
    if (!url.isValid() || url.isEmpty()) {
        return;
    }
    if (behaviour == Behaviour::Clear) {
        clear();
    }

    if (url.scheme() == u"file"_s) {
        auto mimeType = Application::mimeType(url);

        if (mimeType == u"audio/x-mpegurl"_s) {
            m_playlistPath = url.toString();
            addM3uItems(url);
            return;
        }

        if (isVideoOrAudioMimeType(mimeType)) {
            if (behaviour == Behaviour::Clear) {
                if (PlaylistSettings::loadSiblings()) {
                    getSiblingItems(url);
                } else {
                    appendItem(url);
                    setPlayingItem(0);
                }
            }
            if (behaviour == Behaviour::Append) {
                appendItem(url);
            }
            if (behaviour == Behaviour::AppendAndPlay) {
                appendItem(url);
                setPlayingItem(m_playlist.count() - 1);
            }

            return;
        }
    }

    if (url.scheme() == u"http"_s || url.scheme() == u"https"_s) {
        if (Application::isYoutubePlaylist(url)) {
            getYouTubePlaylist(url, behaviour);
        } else {
            if (behaviour == Behaviour::Clear) {
                appendItem(url);
                setPlayingItem(0);
            }
            if (behaviour == Behaviour::Append) {
                appendItem(url);
            }
            if (behaviour == Behaviour::AppendAndPlay) {
                appendItem(url);
                setPlayingItem(m_playlist.count() - 1);
            }
        }
    }
}

void PlaylistModel::appendItem(const QUrl &url)
{
    PlaylistItem item;
    QFileInfo itemInfo(url.toLocalFile());
    auto row{m_playlist.count()};
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
                getHttpItemInfo(url, row);
                ++m_httpItemCounter;
            }
        }
    }

    if (item.url.isEmpty()) {
        return;
    }

    beginInsertRows(QModelIndex(), m_playlist.count(), m_playlist.count());

    m_playlist.append(item);
    Q_EMIT itemAdded(row, item.url.toString());

    endInsertRows();
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

    beginInsertRows(QModelIndex(), 0, siblingFiles.count() - 1);
    for (const auto &file : siblingFiles) {
        QFileInfo fileInfo(file);
        auto fileUrl = QUrl::fromLocalFile(file);
        PlaylistItem item;
        item.url = fileUrl;
        item.filename = fileInfo.fileName();
        item.folderPath = fileInfo.absolutePath();
        m_playlist.append(item);
        // in flatpak the file dialog gives a percent encoded path
        // use toLocalFile to normalize the urls
        if (url.toLocalFile() == fileUrl.toLocalFile()) {
            setPlayingItem(m_playlist.count() - 1);
        }
        Q_EMIT itemAdded(m_playlist.count() - 1, item.url.toString());
    }
    endInsertRows();
}

void PlaylistModel::addM3uItems(const QUrl &url)
{
    if (url.scheme() != u"file"_s || Application::mimeType(url) != u"audio/x-mpegurl"_s) {
        return;
    }

    QFile m3uFile(url.toString(QUrl::PreferLocalFile));
    if (!m3uFile.open(QFile::ReadOnly)) {
        qDebug() << "can't open playlist file";
        return;
    }

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
        addItem(url, Behaviour::Append);

        if (!matchFound && url == QUrl::fromUserInput(GeneralSettings::lastPlayedFile())) {
            setPlayingItem(i);
            matchFound = true;
        }
        ++i;
    }
    m3uFile.close();

    if (!matchFound) {
        setPlayingItem(0);
    }
}

void PlaylistModel::getYouTubePlaylist(const QUrl &url, Behaviour behaviour)
{
    QUrlQuery query{url.query()};
    QString playlistId{query.queryItemValue(u"list"_s)};
    QString videoId{query.queryItemValue(u"v"_s)};
    m_playlistPath = u"https://www.youtube.com/playlist?list=%1"_s.arg(playlistId);

    // use youtube-dl to get the required playlist info as json
    auto ytdlProcess = std::make_shared<QProcess>();
    auto args = QStringList() << u"-J"_s << u"--flat-playlist"_s << m_playlistPath;
    ytdlProcess->setProgram(Application::youtubeDlExecutable());
    ytdlProcess->setArguments(args);
    ytdlProcess->start();

    connect(ytdlProcess.get(), QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [=](int, QProcess::ExitStatus) {
        QString json = QString::fromUtf8(ytdlProcess->readAllStandardOutput());
        QJsonValue entries = QJsonDocument::fromJson(json.toUtf8())[u"entries"_s];
        // QString playlistTitle = QJsonDocument::fromJson(json.toUtf8())[u"title")].toString();
        if (entries.toArray().isEmpty()) {
            return;
        }

        bool matchFound{false};
        for (int i = 0; i < entries.toArray().size(); ++i) {
            auto id = entries[i][u"id"_s].toString();
            auto url = u"https://www.youtube.com/watch?v=%1&list=%2"_s.arg(id, playlistId);
            auto title = entries[i][u"title"_s].toString();
            auto duration = entries[i][u"duration"_s].toDouble();

            PlaylistItem item;
            item.url = QUrl::fromUserInput(url);
            item.filename = !title.isEmpty() ? title : url;
            item.mediaTitle = !title.isEmpty() ? title : url;
            item.formattedDuration = Application::formatTime(duration);
            item.duration = duration;

            beginInsertRows(QModelIndex(), m_playlist.count(), m_playlist.count());
            m_playlist.append(item);
            Q_EMIT itemAdded(i, item.url.toString());
            endInsertRows();

            if (videoId.isEmpty()) {
                // when videoId is not available check if GeneralSettings::lastPlayedFile()
                // is part of the playlist and set the matching item as playing
                if (GeneralSettings::lastPlayedFile().contains(id) && !matchFound && behaviour == Behaviour::Clear) {
                    setPlayingItem(i);
                    matchFound = true;
                }
            } else {
                // when videoId is available set the item with the same id as playing
                if (videoId == id && !matchFound && behaviour == Behaviour::Clear) {
                    setPlayingItem(i);
                    matchFound = true;
                }
            }
        }

        if (!matchFound && behaviour == Behaviour::Clear) {
            setPlayingItem(0);
        }
    });
}

void PlaylistModel::getHttpItemInfo(const QUrl &url, uint row)
{
    auto ytdlProcess = std::make_shared<QProcess>();
    ytdlProcess->setProgram(Application::youtubeDlExecutable());
    ytdlProcess->setArguments(QStringList() << u"-j"_s << url.toString());
    ytdlProcess->start();

    connect(ytdlProcess.get(), QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [=](int, QProcess::ExitStatus) {
        QString json = QString::fromUtf8(ytdlProcess->readAllStandardOutput());
        QString title = QJsonDocument::fromJson(json.toUtf8())[u"title"_s].toString();
        auto duration = QJsonDocument::fromJson(json.toUtf8())[u"duration"_s].toDouble();
        if (title.isEmpty()) {
            // todo: log if can't get title
            return;
        }
        m_playlist[row].mediaTitle = title;
        m_playlist[row].filename = title;
        m_playlist[row].formattedDuration = Application::formatTime(duration);
        m_playlist[row].duration = duration;

        --m_httpItemCounter;

        Q_EMIT dataChanged(index(row, 0), index(row, 0));
    });
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

    auto currentIndex = mapFromSource(model->index(model->m_playingItem, 0)).row();
    auto nextIndex = currentIndex + 1;

    if (nextIndex < rowCount()) {
        model->setPlayingItem(mapToSource(index(nextIndex, 0)).row());
    } else {
        setPlayingItem(0);
    }
}

void PlaylistProxyModel::playPrevious()
{
    auto model = qobject_cast<PlaylistModel *>(sourceModel());

    auto currentIndex = mapFromSource(model->index(model->m_playingItem, 0)).row();
    auto previousIndex = currentIndex - 1;

    if (previousIndex >= 0) {
        model->setPlayingItem(mapToSource(index(previousIndex, 0)).row());
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
    model->m_playlist.removeAt(sourceRow);
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
            model->m_playlist.removeAt(sourceRow);
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
