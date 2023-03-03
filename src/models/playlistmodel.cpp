/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "playlistmodel.h"
#include "application.h"
#include "generalsettings.h"
#include "global.h"
#include "playlistitem.h"
#include "playlistsettings.h"
#include "worker.h"

#include <KFileItem>
#include <kio_version.h>
#if KIO_VERSION >= QT_VERSION_CHECK(5, 100, 0)
#include <KIO/DeleteOrTrashJob>
#endif
#include <KIO/OpenFileManagerWindowJob>
#include <KIO/RenameFileDialog>
#include <KLocalizedString>

#include <QClipboard>
#include <QCollator>
#include <QDirIterator>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QProcess>
#include <QUrl>

PlayListModel::PlayListModel(QObject *parent)
    : QAbstractListModel(parent)
{
    connect(this, &PlayListModel::itemAdded, Worker::instance(), &Worker::getMetaData);

#if KCONFIG_VERSION >= QT_VERSION_CHECK(5, 89, 0)
    connect(Worker::instance(), &Worker::metaDataReady, this, [=](int i, KFileMetaData::PropertyMultiMap metaData) {
#else
    connect(Worker::instance(), &Worker::metaDataReady, this, [=](int i, KFileMetaData::PropertyMap metaData) {
#endif
        auto duration = metaData.value(KFileMetaData::Property::Duration).toInt();
        auto title = metaData.value(KFileMetaData::Property::Title).toString();

        m_playlist[i]->setDuration(Application::formatTime(duration));
        m_playlist[i]->setMediaTitle(title);

        Q_EMIT dataChanged(index(i, 0), index(i, 0));
    });
}

int PlayListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_playlist.size();
}

QVariant PlayListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || m_playlist.empty())
        return QVariant();

    auto playListItem = m_playlist.at(index.row());
    switch (role) {
    case NameRole:
        return QVariant(playListItem->fileName());
    case TitleRole:
        return playListItem->mediaTitle().isEmpty() ? QVariant(playListItem->fileName()) : QVariant(playListItem->mediaTitle());
    case PathRole:
        return QVariant(playListItem->filePath());
    case DurationRole:
        return QVariant(playListItem->duration());
    case PlayingRole:
        return QVariant(m_playingItem == index.row());
    case FolderPathRole:
        return QVariant(playListItem->folderPath());
    case IsLocalRole:
        return QVariant(!playListItem->filePath().startsWith("http"));
    }

    return QVariant();
}

QHash<int, QByteArray> PlayListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[TitleRole] = "title";
    roles[PathRole] = "path";
    roles[FolderPathRole] = "folderPath";
    roles[DurationRole] = "duration";
    roles[PlayingRole] = "isPlaying";
    roles[IsLocalRole] = "isLocal";
    return roles;
}

void PlayListModel::getSiblingItems(QString path)
{
    clear();
    QUrl openedUrl(path);
    if (openedUrl.scheme().isEmpty()) {
        openedUrl.setScheme("file");
    }
    QFileInfo openedFileInfo(openedUrl.toLocalFile());
    if (openedFileInfo.exists() && openedFileInfo.isFile()) {
        QStringList siblingFiles;
        QDirIterator it(openedFileInfo.absolutePath(), QDir::Files, QDirIterator::NoIteratorFlags);
        while (it.hasNext()) {
            QString siblingFile = it.next();
            QFileInfo siblingFileInfo(siblingFile);
            // toPercentEncoding() is needed to get the correct mime type
            // for files containing the # character and probably other characters too
            // if the mime type is wrong the file will be ignored
            QUrl siblingUrl(siblingFile.toUtf8().toPercentEncoding());
            siblingUrl.setScheme(openedUrl.scheme());
            QString mimeType = Application::mimeType(siblingFile);
            if (siblingFileInfo.exists() && (mimeType.startsWith("video/") || mimeType.startsWith("audio/")) && mimeType != QStringLiteral("audio/x-mpegurl")) {
                siblingFiles.append(siblingFileInfo.absoluteFilePath());
            }
        }

        QCollator collator;
        collator.setNumericMode(true);
        std::sort(siblingFiles.begin(), siblingFiles.end(), collator);

        beginInsertRows(QModelIndex(), 0, siblingFiles.count() - 1);
        for (int i = 0; i < siblingFiles.count(); ++i) {
            auto item = new PlayListItem(siblingFiles.at(i), this);
            m_playlist.append(item);
            if (openedUrl.toString(QUrl::PreferLocalFile) == siblingFiles.at(i)) {
                setPlayingItem(i);
            }
            Q_EMIT itemAdded(i, item->filePath());
        }
        endInsertRows();
    }
}

void PlayListModel::appendItem(QString path)
{
    path = QUrl(path).toLocalFile().isEmpty() ? path : QUrl(path).toLocalFile();
    PlayListItem *item{nullptr};
    QFileInfo itemInfo(path);
    int row{m_playlist.count()};
    if (itemInfo.exists() && itemInfo.isFile()) {
        QString mimeType = Application::mimeType(itemInfo.absoluteFilePath());
        if (mimeType.startsWith("video/") || mimeType.startsWith("audio/")) {
            item = new PlayListItem(itemInfo.absoluteFilePath(), this);
        }
    } else {
        if (path.startsWith("http")) {
            item = new PlayListItem(path, this);
            // causes issues with lots of links
            // getHttpItemInfo(path, row);
        }
    }

    if (item == nullptr) {
        return;
    }

    beginInsertRows(QModelIndex(), m_playlist.count(), m_playlist.count());

    m_playlist.append(item);
    Q_EMIT itemAdded(row, item->filePath());

    endInsertRows();
}

void PlayListModel::removeItem(int index)
{
    beginRemoveRows(QModelIndex(), index, index);
    m_playlist.removeAt(index);
    endRemoveRows();
    Q_EMIT itemRemoved(index, getPath(index));
}

Playlist PlayListModel::items() const
{
    return m_playlist;
}

void PlayListModel::getHttpItemInfo(const QString &url, int row)
{
    auto ytdlProcess = new QProcess();
    ytdlProcess->setProgram(Application::youtubeDlExecutable());
    ytdlProcess->setArguments(QStringList() << "-j" << url);
    ytdlProcess->start();

    QObject::connect(ytdlProcess, (void(QProcess::*)(int, QProcess::ExitStatus)) & QProcess::finished, this, [=](int, QProcess::ExitStatus) {
        QString json = ytdlProcess->readAllStandardOutput();
        QString title = QJsonDocument::fromJson(json.toUtf8())["title"].toString();
        int duration = QJsonDocument::fromJson(json.toUtf8())["duration"].toInt();
        if (title.isEmpty()) {
            // todo: log if can't get title
            return;
        }
        m_playlist.at(row)->setMediaTitle(title);
        m_playlist.at(row)->setFileName(title);
        m_playlist.at(row)->setDuration(Application::formatTime(duration));
        if (m_emitOpened) {
            Q_EMIT opened(title, url);
            m_emitOpened = false;
        }
        Q_EMIT dataChanged(index(row, 0), index(row, 0));
    });
}

Playlist PlayListModel::getPlayList()
{
    return m_playlist;
}

int PlayListModel::getPlayingItem() const
{
    return m_playingItem;
}

void PlayListModel::clear()
{
    m_playingItem = 0;
    qDeleteAll(m_playlist);
    beginResetModel();
    m_playlist.clear();
    endResetModel();
}

void PlayListModel::openM3uFile(const QString &path)
{
    if (Application::mimeType(path) != QStringLiteral("audio/x-mpegurl")) {
        return;
    }

    QUrl playlistUrl(path);
    QFile m3uFile(playlistUrl.toString(QUrl::PreferLocalFile));
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

        QUrl url(line);
        QString item;
        if (!url.scheme().isEmpty()) {
            item = url.toString(QUrl::PreferLocalFile);
        } else {
            // figure out if it's a relative path
            item = QUrl::fromUserInput(line, QFileInfo(m3uFile).absolutePath()).toString(QUrl::PreferLocalFile);
        }
        appendItem(item);
        if (item == QUrl(GeneralSettings::lastPlayedFile()).toString(QUrl::PreferLocalFile)) {
            setPlayingItem(i);
            matchFound = true;
        }
        ++i;
    }
    if (!matchFound) {
        setPlayingItem(0);
    }
    m3uFile.close();
}

void PlayListModel::openFile(const QString &path)
{
    clear();
    QUrl url(path);
    if (url.scheme() == "file" || url.scheme() == QString()) {
        auto mimeType = Application::mimeType(path);
        if (mimeType == QStringLiteral("audio/x-mpegurl")) {
            openM3uFile(path);
            Q_EMIT opened(QFileInfo(path).fileName(), url.toString(QUrl::PreferLocalFile));

            GeneralSettings::setLastPlaylist(path);
            GeneralSettings::self()->save();
            return;
        }
        if (mimeType.startsWith("video/") || mimeType.startsWith("audio/")) {
            if (PlaylistSettings::loadSiblings()) {
                getSiblingItems(path);
            } else {
                appendItem(path);
                setPlayingItem(0);
            }

            Q_EMIT opened(QFileInfo(path).fileName(), url.toString(QUrl::PreferLocalFile));

            GeneralSettings::setLastPlayedFile(path);
            // clear the lastPlaylist so when the player opens without a file
            // it opens the lastPlayedFile instead of the last playlist
            GeneralSettings::setLastPlaylist(QString());
            GeneralSettings::self()->save();
            return;
        }
    }
    if (url.scheme().startsWith("http")) {
        if (Application::isYoutubePlaylist(path)) {
            getYouTubePlaylist(path);
            GeneralSettings::setLastPlaylist(path);
            GeneralSettings::self()->save();
        } else {
            // emit opened signal only when actually opening an url
            m_emitOpened = true;
            appendItem(path);
            setPlayingItem(0);
            GeneralSettings::setLastPlayedFile(path);
            GeneralSettings::setLastPlaylist(QString());
            GeneralSettings::self()->save();
        }
    }
}

QString PlayListModel::getPath(int index)
{
    // when restoring a youtube playlist
    // ensure the requested path is valid
    if (m_playlist.isEmpty()) {
        return QString();
    }
    if (index == -1) {
        return m_playlist[m_playingItem]->filePath();
    }
    if (m_playlist.size() <= index) {
        return m_playlist[0]->filePath();
    }
    return m_playlist[index]->filePath();
}

PlayListItem *PlayListModel::getItem(int index)
{
    if (m_playlist.size() <= index) {
        return m_playlist[0];
    }
    return m_playlist[index];
}

void PlayListModel::setPlayingItem(int i)
{
    if (i == -1) {
        return;
    }

    int previousItem = m_playingItem;
    m_playingItem = i;
    Q_EMIT dataChanged(index(previousItem, 0), index(previousItem, 0));
    Q_EMIT dataChanged(index(i, 0), index(i, 0));
    Q_EMIT playingItemChanged();
}

void PlayListModel::getYouTubePlaylist(const QString &path)
{
    // use youtube-dl to get the required playlist info as json
    auto ytdlProcess = new QProcess();
    ytdlProcess->setProgram(Application::youtubeDlExecutable());
    ytdlProcess->setArguments(QStringList() << "-J"
                                            << "--flat-playlist" << path);
    ytdlProcess->start();

    QObject::connect(ytdlProcess, (void(QProcess::*)(int, QProcess::ExitStatus)) & QProcess::finished, this, [=](int, QProcess::ExitStatus) {
        QString json = ytdlProcess->readAllStandardOutput();
        QJsonValue entries = QJsonDocument::fromJson(json.toUtf8())["entries"];
        QString playlistTitle = QJsonDocument::fromJson(json.toUtf8())["title"].toString();
        if (entries.toArray().isEmpty()) {
            Q_EMIT Global::instance()->error(i18nc("@info", "Playlist is empty", playlistTitle));
            return;
        }

        bool matchFound{false};
        for (int i = 0; i < entries.toArray().size(); ++i) {
            auto url = QString("https://youtu.be/%1").arg(entries[i]["id"].toString());
            auto title = entries[i]["title"].toString();
            auto duration = entries[i]["duration"].toDouble();

            auto video = new PlayListItem(url, this);
            video->setMediaTitle(!title.isEmpty() ? title : url);
            video->setFileName(!title.isEmpty() ? title : url);
            video->setDuration(Application::formatTime(duration));

            beginInsertRows(QModelIndex(), m_playlist.count(), m_playlist.count());
            m_playlist.append(video);
            Q_EMIT itemAdded(i, video->filePath());
            endInsertRows();

            if (GeneralSettings::lastPlayedFile().contains(entries[i]["id"].toString())) {
                setPlayingItem(i);
                matchFound = true;
            }
        }
        Q_EMIT opened(playlistTitle, path);
        if (!matchFound) {
            setPlayingItem(0);
        }
    });
}

PlayListProxyModel::PlayListProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setDynamicSortFilter(true);
}

void PlayListProxyModel::sortItems(Sort sortMode)
{
    switch (sortMode) {
    case NameAscending: {
        setSortRole(PlayListModel::PathRole);
        sort(0, Qt::AscendingOrder);
        break;
    }
    case NameDescending: {
        setSortRole(PlayListModel::PathRole);
        sort(0, Qt::DescendingOrder);
        break;
    }
    case DurationAscending: {
        setSortRole(PlayListModel::DurationRole);
        sort(0, Qt::AscendingOrder);
        break;
    }
    case DurationDescending: {
        setSortRole(PlayListModel::DurationRole);
        sort(0, Qt::DescendingOrder);
        break;
    }
    }
}

int PlayListProxyModel::getPlayingItem()
{
    auto model = qobject_cast<PlayListModel *>(sourceModel());
    return mapFromSource(index(model->getPlayingItem(), 0)).row();
}

void PlayListProxyModel::setPlayingItem(int i)
{
    auto model = qobject_cast<PlayListModel *>(sourceModel());
    model->setPlayingItem(mapToSource(index(i, 0)).row());
}

void PlayListProxyModel::playNext()
{
    auto model = qobject_cast<PlayListModel *>(sourceModel());

    auto currentIndex = mapFromSource(model->index(model->getPlayingItem(), 0)).row();
    auto nextIndex = currentIndex + 1;

    if (nextIndex < rowCount()) {
        model->setPlayingItem(mapToSource(index(nextIndex, 0)).row());
    } else {
        setPlayingItem(0);
    }
}

void PlayListProxyModel::playPrevious()
{
    auto model = qobject_cast<PlayListModel *>(sourceModel());

    auto currentIndex = mapFromSource(model->index(model->getPlayingItem(), 0)).row();
    auto previousIndex = currentIndex - 1;

    if (previousIndex >= 0) {
        model->setPlayingItem(mapToSource(index(previousIndex, 0)).row());
    }
}

void PlayListProxyModel::saveM3uFile(const QString &path)
{
    QUrl url(path);
    QFile m3uFile(url.toString(QUrl::PreferLocalFile));
    if (!m3uFile.open(QFile::WriteOnly)) {
        return;
    }
    for (int i{0}; i < rowCount(); ++i) {
        QString itemPath = data(index(i, 0), PlayListModel::PathRole).toString();
        m3uFile.write(itemPath.toUtf8().append("\n"));
    }
    m3uFile.close();
}

void PlayListProxyModel::highlightInFileManager(int row)
{
    QString path = data(index(row, 0), PlayListModel::PathRole).toString();
    QUrl url(path);
    if (url.scheme().isEmpty()) {
        url.setScheme(QStringLiteral("file"));
    }
    KIO::highlightInFileManager({url});
}

void PlayListProxyModel::removeItem(int row)
{
    auto model = qobject_cast<PlayListModel *>(sourceModel());

    auto sourceRow = mapFromSource(model->index(row)).row();
    model->removeItem(sourceRow);
}

void PlayListProxyModel::renameFile(int row)
{
    QString path = data(index(row, 0), PlayListModel::PathRole).toString();
    QUrl url(path);
    if (url.scheme().isEmpty()) {
        url.setScheme(QStringLiteral("file"));
    }
    KFileItem item(url);
    auto renameDialog = new KIO::RenameFileDialog(KFileItemList({item}), nullptr);
    renameDialog->open();

    connect(renameDialog, &KIO::RenameFileDialog::renamingFinished, this, [=](const QList<QUrl> &urls) {
        auto model = qobject_cast<PlayListModel *>(sourceModel());
        auto sourceRow = mapToSource(index(row, 0)).row();
        auto item = model->getPlayList().at(sourceRow);
        item->setFilePath(urls.first().path());
        item->setFileName(urls.first().fileName());

        Q_EMIT dataChanged(index(row, 0), index(row, 0));
    });
}

void PlayListProxyModel::trashFile(int row)
{
#if KIO_VERSION >= QT_VERSION_CHECK(5, 100, 0)
    QList<QUrl> urls;
    QString path = data(index(row, 0), PlayListModel::PathRole).toString();
    QUrl url(path);
    if (url.scheme().isEmpty()) {
        url.setScheme(QStringLiteral("file"));
    }
    urls << url;
    auto *job = new KIO::DeleteOrTrashJob(urls, KIO::AskUserActionInterface::Trash, KIO::AskUserActionInterface::DefaultConfirmation, this);
    job->start();

    connect(job, &KJob::result, this, [=]() {
        if (job->error() == 0) {
            auto model = qobject_cast<PlayListModel *>(sourceModel());
            auto sourceRow = mapToSource(index(row, 0)).row();
            model->removeItem(sourceRow);
        }
    });
#endif
}

void PlayListProxyModel::copyFileName(int row)
{
    auto model = qobject_cast<PlayListModel *>(sourceModel());
    auto item = model->getPlayList().at(row);
    QGuiApplication::clipboard()->setText(item->fileName());
}

void PlayListProxyModel::copyFilePath(int row)
{
    auto model = qobject_cast<PlayListModel *>(sourceModel());
    auto item = model->getPlayList().at(row);
    QGuiApplication::clipboard()->setText(item->filePath());
}

#include "moc_playlistmodel.cpp"
