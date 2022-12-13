/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "playlistmodel.h"
#include "playlistitem.h"
#include "application.h"
#include "global.h"
#include "playlistsettings.h"
#include "worker.h"

#include <KFileItem>
#include <KLocalizedString>
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
    connect(this, &PlayListModel::itemAdded,
            Worker::instance(), &Worker::getMetaData);

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
        return playListItem->mediaTitle().isEmpty()
                ? QVariant(playListItem->fileName())
                : QVariant(playListItem->mediaTitle());
    case PathRole:
        return QVariant(playListItem->filePath());
    case DurationRole:
        return QVariant(playListItem->duration());
    case PlayingRole:
        return QVariant(playListItem->isPlaying());
    case FolderPathRole:
        return QVariant(playListItem->folderPath());
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
    return roles;
}

void PlayListModel::getSiblingItems(QString path)
{
    clear();
    QUrl url {path};
    bool isUrl = url.scheme().startsWith(QStringLiteral("http"));
    if (!PlaylistSettings::loadSiblings() || isUrl) {
        appendItem(path);
        return;
    }

    QUrl openedUrl(path);
    if (openedUrl.scheme().isEmpty()) {
        openedUrl.setScheme("file");
    }
    QFileInfo openedFileInfo(openedUrl.toLocalFile());
    QStringList siblingFiles;
    if (openedFileInfo.exists() && openedFileInfo.isFile()) {
        QDirIterator it(openedFileInfo.absolutePath(), QDir::Files, QDirIterator::NoIteratorFlags);
        while (it.hasNext()) {
            QString siblingFile = it.next();
            QFileInfo siblingFileInfo(siblingFile);
            QUrl siblingUrl(siblingFile);
            siblingUrl.setScheme(openedUrl.scheme());
            QString mimeType = Application::mimeType(siblingUrl);
            if (siblingFileInfo.exists() && (mimeType.startsWith("video/") || mimeType.startsWith("audio/"))) {
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
            if (path == siblingFiles.at(i)) {
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
    PlayListItem *item {nullptr};
    QFileInfo itemInfo(path);
    int row {m_playlist.count()};
    if (itemInfo.exists() && itemInfo.isFile()) {
        QString mimeType = Application::mimeType(QUrl(itemInfo.absoluteFilePath()));
        if (mimeType.startsWith("video/") || mimeType.startsWith("audio/")) {
            item = new PlayListItem(itemInfo.absoluteFilePath(), this);
        }
    } else {
        if (path.startsWith("http")) {
            item = new PlayListItem(path, this);
            getHttpItemInfo(path, row);
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

void PlayListModel::playNext()
{
    int i = m_playingItem + 1;
    if (i < rowCount()) {
        setPlayingItem(i);
    } else {
        setPlayingItem(0);
    }
}

void PlayListModel::playPrevious()
{
    int i = m_playingItem - 1;
    if (i >= 0) {
        setPlayingItem(i);
    }
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

    QObject::connect(ytdlProcess, (void (QProcess::*)(int,QProcess::ExitStatus))&QProcess::finished,
                     this, [=](int, QProcess::ExitStatus) {
        QString json = ytdlProcess->readAllStandardOutput();
        QString title = QJsonDocument::fromJson(json.toUtf8())["title"].toString();
        int duration = QJsonDocument::fromJson(json.toUtf8())["duration"].toInt();
        if (title.isEmpty()) {
            Q_EMIT Global::instance()->error(i18nc("@info", "No title found for url %1", url));
            return;
        }
        m_playlist.at(row)->setMediaTitle(title);
        m_playlist.at(row)->setDuration(Application::formatTime(duration));
        Q_EMIT dataChanged(index(row, 0), index(row, 0));
    });
}

Playlist PlayListModel::getPlayList() const
{
    return m_playlist;
}

void PlayListModel::setPlayList(const Playlist &playList)
{
    beginInsertRows(QModelIndex(), 0, playList.size() - 1);
    m_playlist = playList;
    endInsertRows();
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
    QUrl url(path);
    QFile m3uFile(url.toString(QUrl::PreferLocalFile));
    if (!m3uFile.open(QFile::ReadOnly)) {
        qDebug() << "can't open playlist file";
        return;
    }
    while (!m3uFile.atEnd()) {
        QByteArray line = QByteArray::fromPercentEncoding(m3uFile.readLine().simplified());
        // ignore comments
        if (line.startsWith("#")) {
            continue;
        }

        QUrl url(line);
        if (!url.scheme().isEmpty()) {
            appendItem(url.toString());
        } else {
            // figure out if it's a relative path
            appendItem(QUrl::fromUserInput(line, QFileInfo(path).absolutePath()).toString());
        }
    }
    setPlayingItem(0);
    m3uFile.close();
}

void PlayListModel::saveM3uFile(const QString &path)
{
    QUrl url(path);
    QFile m3uFile(url.toString(QUrl::PreferLocalFile));
    if (!m3uFile.open(QFile::WriteOnly)) {
        return;
    }
    for (const auto &item : m_playlist) {
        m3uFile.write(item->filePath().toUtf8().append("\n"));
    }
    m3uFile.close();
}

QString PlayListModel::getPath(int index)
{
    // when restoring a youtube playlist
    // ensure the requested path is valid
    if (m_playlist.isEmpty()) {
        return QString();
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
    // unset current playing item
    m_playlist[m_playingItem]->setIsPlaying(false);
    Q_EMIT dataChanged(index(m_playingItem, 0), index(m_playingItem, 0));

    // set new playing item
    m_playlist[i]->setIsPlaying(true);
    Q_EMIT dataChanged(index(i, 0), index(i, 0));

    m_playingItem = i;
    Q_EMIT playingItemChanged();
}
