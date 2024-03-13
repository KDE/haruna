/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "playlistmodel.h"

#include <KFileItem>
#include <KFileMetaData/Properties>
#include <KIO/DeleteOrTrashJob>
#include <KIO/OpenFileManagerWindowJob>
#include <KIO/RenameFileDialog>
#include <kio_version.h>

#include <QClipboard>
#include <QCollator>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QProcess>

#include "application.h"
#include "generalsettings.h"
#include "playlistsettings.h"
#include "worker.h"

PlaylistModel::PlaylistModel(QObject *parent)
    : QAbstractListModel(parent)
{
    connect(this, &PlaylistModel::itemAdded, Worker::instance(), &Worker::getMetaData);

    connect(Worker::instance(), &Worker::metaDataReady, this, [=](int i, KFileMetaData::PropertyMultiMap metaData) {
        if (m_playlist.isEmpty()) {
            return;
        }
        auto duration = metaData.value(KFileMetaData::Property::Duration).toInt();
        auto title = metaData.value(KFileMetaData::Property::Title).toString();

        m_playlist[i].formattedDuration = Application::formatTime(duration);
        m_playlist[i].duration = duration;
        m_playlist[i].mediaTitle = title;

        Q_EMIT dataChanged(index(i, 0), index(i, 0));
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
        return QVariant(m_playingItem == index.row());
    case FolderPathRole:
        return QVariant(item.folderPath);
    case IsLocalRole:
        return QVariant(!item.url.scheme().startsWith(QStringLiteral("http")));
    }

    return QVariant();
}

QHash<int, QByteArray> PlaylistModel::roleNames() const
{
    QHash<int, QByteArray> roles = {
        {NameRole, "name"},
        {TitleRole, "title"},
        {PathRole, "path"},
        {FolderPathRole, "folderPath"},
        {DurationRole, "duration"},
        {PlayingRole, "isPlaying"},
        {IsLocalRole, "isLocal"},
    };
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

    if (url.scheme() == QStringLiteral("file")) {
        auto mimeType = Application::mimeType(url);

        if (mimeType == QStringLiteral("audio/x-mpegurl")) {
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
            } else {
                appendItem(url);
            }

            return;
        }
    }

    if (url.scheme() == QStringLiteral("http") || url.scheme() == QStringLiteral("https")) {
        if (Application::isYoutubePlaylist(url.toString())) {
            m_playlistPath = url.toString();
            getYouTubePlaylist(url, behaviour);
        } else {
            if (behaviour == Behaviour::Clear) {
                appendItem(url);
                setPlayingItem(0);
            } else {
                appendItem(url);
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
        if (url.scheme().startsWith(QStringLiteral("http"))) {
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
    QDirIterator it(openedFileInfo.absolutePath(), QDir::Files, QDirIterator::NoIteratorFlags);
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
    if (url.scheme() != QStringLiteral("file") || Application::mimeType(url) != QStringLiteral("audio/x-mpegurl")) {
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

        auto url = QUrl::fromUserInput(QString::fromUtf8(line));
        if (!url.scheme().isEmpty()) {
            addItem(url, Behaviour::Append);
        } else {
            // figure out if it's a relative path
            url = QUrl::fromUserInput(QString::fromUtf8(line), QFileInfo(m3uFile).absolutePath());
            addItem(url, Behaviour::Append);
        }

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
    // use youtube-dl to get the required playlist info as json
    auto ytdlProcess = std::make_shared<QProcess>();
    auto args = QStringList() << QStringLiteral("-J") << QStringLiteral("--flat-playlist") << url.toString();
    ytdlProcess->setProgram(Application::youtubeDlExecutable());
    ytdlProcess->setArguments(args);
    ytdlProcess->start();

    connect(ytdlProcess.get(), QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [=](int, QProcess::ExitStatus) {
        QString json = QString::fromUtf8(ytdlProcess->readAllStandardOutput());
        QJsonValue entries = QJsonDocument::fromJson(json.toUtf8())[QStringLiteral("entries")];
        // QString playlistTitle = QJsonDocument::fromJson(json.toUtf8())[QStringLiteral("title")].toString();
        if (entries.toArray().isEmpty()) {
            return;
        }

        bool matchFound{false};
        for (int i = 0; i < entries.toArray().size(); ++i) {
            auto id = entries[i][QStringLiteral("id")].toString();
            auto url = QStringLiteral("https://youtu.be/%1").arg(entries[i][QStringLiteral("id")].toString());
            auto title = entries[i][QStringLiteral("title")].toString();
            auto duration = entries[i][QStringLiteral("duration")].toDouble();

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

            if (GeneralSettings::lastPlayedFile().contains(id) && behaviour == Behaviour::Clear) {
                setPlayingItem(i);
                matchFound = true;
            }
        }

        if (!matchFound && behaviour == Behaviour::Clear) {
            setPlayingItem(0);
        }
    });
}

void PlaylistModel::getHttpItemInfo(const QUrl &url, int row)
{
    auto ytdlProcess = std::make_shared<QProcess>();
    ytdlProcess->setProgram(Application::youtubeDlExecutable());
    ytdlProcess->setArguments(QStringList() << QStringLiteral("-j") << url.toString());
    ytdlProcess->start();

    connect(ytdlProcess.get(), QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [=](int, QProcess::ExitStatus) {
        QString json = QString::fromUtf8(ytdlProcess->readAllStandardOutput());
        QString title = QJsonDocument::fromJson(json.toUtf8())[QStringLiteral("title")].toString();
        auto duration = QJsonDocument::fromJson(json.toUtf8())[QStringLiteral("duration")].toDouble();
        if (title.isEmpty()) {
            // todo: log if can't get title
            return;
        }
        m_playlist[row].mediaTitle = title;
        m_playlist[row].filename = title;
        m_playlist[row].formattedDuration = Application::formatTime(duration);
        m_playlist[row].duration = duration;

        Q_EMIT dataChanged(index(row, 0), index(row, 0));
    });
}

bool PlaylistModel::isVideoOrAudioMimeType(const QString &mimeType)
{
    // clang-format off
    return (mimeType.startsWith(QStringLiteral("video/"))
            || mimeType.startsWith(QStringLiteral("audio/"))
            || mimeType == QStringLiteral("application/vnd.rn-realmedia"))
            && mimeType != QStringLiteral("audio/x-mpegurl");
    // clang-format on
}

void PlaylistModel::setPlayingItem(int i)
{
    if (i < -1 || i >= m_playlist.size()) {
        return;
    }

    int previousItem{m_playingItem};
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

int PlaylistProxyModel::getPlayingItem()
{
    auto model = qobject_cast<PlaylistModel *>(sourceModel());
    return mapFromSource(model->index(model->m_playingItem, 0)).row();
}

void PlaylistProxyModel::setPlayingItem(int i)
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

void PlaylistProxyModel::highlightInFileManager(int row)
{
    QString path = data(index(row, 0), PlaylistModel::PathRole).toString();
    KIO::highlightInFileManager({QUrl::fromUserInput(path)});
}

void PlaylistProxyModel::removeItem(int row)
{
    auto model = qobject_cast<PlaylistModel *>(sourceModel());

    auto sourceRow = mapFromSource(model->index(row)).row();

    beginRemoveRows(QModelIndex(), sourceRow, sourceRow);
    model->m_playlist.removeAt(sourceRow);
    endRemoveRows();
}

void PlaylistProxyModel::renameFile(int row)
{
    QString path = data(index(row, 0), PlaylistModel::PathRole).toString();
    QUrl url(path);
    if (url.scheme().isEmpty()) {
        url.setScheme(QStringLiteral("file"));
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

void PlaylistProxyModel::trashFile(int row)
{
    QList<QUrl> urls;
    QString path = data(index(row, 0), PlaylistModel::PathRole).toString();
    QUrl url(path);
    if (url.scheme().isEmpty()) {
        url.setScheme(QStringLiteral("file"));
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

void PlaylistProxyModel::copyFileName(int row)
{
    auto model = qobject_cast<PlaylistModel *>(sourceModel());
    auto item = model->m_playlist.at(row);
    QGuiApplication::clipboard()->setText(item.filename);
}

void PlaylistProxyModel::copyFilePath(int row)
{
    auto model = qobject_cast<PlaylistModel *>(sourceModel());
    auto item = model->m_playlist.at(row);
    QGuiApplication::clipboard()->setText(item.url.toString());
}

QString PlaylistProxyModel::getFilePath(int row)
{
    auto model = qobject_cast<PlaylistModel *>(sourceModel());
    auto item = model->m_playlist.at(row);
    return item.url.toString();
}

#include "moc_playlistmodel.cpp"
