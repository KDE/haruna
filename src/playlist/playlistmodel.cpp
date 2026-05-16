/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "playlistmodel.h"

#include <QCollator>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonObject>

#include <KFileMetaData/ExtractorCollection>
#include <KFileMetaData/SimpleExtractionResult>

#include <random>

#include "../database.h"
#include "generalsettings.h"
#include "miscutils.h"
#include "playlistsettings.h"
#include "youtube.h"

using namespace Qt::StringLiterals;

PlaylistModel::PlaylistModel(QObject *parent)
    : QAbstractListModel(parent)
{
    if (PlaylistSettings::randomPlayback()) {
        shuffleIndexes();
    }

    connect(PlaylistSettings::self(), &PlaylistSettings::RandomPlaybackChanged, this, [this]() {
        if (PlaylistSettings::randomPlayback()) {
            shuffleIndexes();
        }
    });
    connect(this, &PlaylistModel::itemAdded, this, &PlaylistModel::getMetaData, Qt::QueuedConnection);
    connect(this, &PlaylistModel::metaDataReady, this, &PlaylistModel::onMetaDataReady, Qt::QueuedConnection);
    connect(&youtube, &YouTube::playlistRetrieved, this, &PlaylistModel::addYouTubePlaylist);
    connect(&youtube, &YouTube::videoInfoRetrieved, this, &PlaylistModel::updateFileInfo);
    connect(&youtube, &YouTube::error, MiscUtils::instance(), &MiscUtils::error);
}

PlaylistModel::~PlaylistModel()
{
    m_threadPool.clear();
    m_threadPool.waitForDone();
}

int PlaylistModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_playlist.count();
}

QVariant PlaylistModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }

    auto item = m_playlist.at(index.row());
    switch (role) {
    case NameRole:
        return item.filename;
    case PathRole:
        return item.url;
    case DurationRole:
        return item.formattedDuration;
    case DateRole:
        return item.modifiedDate;
    case FileSizeRole:
        return item.fileSize;
    case TypeRole:
        return item.fileType;
    case ExtensionRole:
        return item.extension;
    case PlayingRole:
        return static_cast<int>(m_playingItem) == index.row() && m_isPlaying;
    case FolderPathRole:
        return item.folderPath;
    case DirNameRole:
        return item.dirName;
    case IsLocalRole:
        return !item.url.scheme().startsWith(u"http"_s);
    case SectionRole:
        return {}; // Should be handled within PlaylistSortProxyModel
    case PlaybackPositionRole:
        return item.playbackPosition;
    // Audio
    case TrackNoRole:
        return item.audio.trackNo;
    case DiscNoRole:
        return item.audio.discNo;
    case TitleRole:
        return item.mediaTitle.isEmpty() ? item.filename : item.mediaTitle;
    case ReleaseYearRole:
        return item.audio.releaseYear;
    case GenreRole:
        return item.audio.genre;
    case AlbumRole:
        return item.audio.album;
    case ArtistRole:
        return item.audio.artist;
    case AlbumArtistRole:
        return item.audio.albumArtist;
    case ComposerRole:
        return item.audio.composer;
    case LyricistRole:
        return item.audio.lyricist;
    case AudioCodecRole:
        return item.audio.audioCodec;
    case SampleRateRole:
        return item.audio.sampleRate;
    case BitrateRole:
        return item.audio.bitrate;
    case FramerateRole:
        return item.video.frameRate;
    // Video
    case VideoCodecRole:
        return item.video.videoCodec;
    case OrientationRole:
        return item.video.displayedOrientation;
    case DisplayedHeightRole:
        return item.video.displayHeight;
    case DisplayedWidthRole:
        return item.video.displayWidth;
    }

    return {};
}

QHash<int, QByteArray> PlaylistModel::roleNames() const
{
    // clang-format off
    static QHash<int, QByteArray> roles = {
        {NameRole,              QByteArrayLiteral("name")},
        {TitleRole,             QByteArrayLiteral("title")},
        {PathRole,              QByteArrayLiteral("path")},
        {FolderPathRole,        QByteArrayLiteral("folderPath")},
        {DirNameRole,           QByteArrayLiteral("dirName")},
        {DurationRole,          QByteArrayLiteral("duration")},
        {DateRole,              QByteArrayLiteral("date")},
        {FileSizeRole,          QByteArrayLiteral("fileSize")},
        {TypeRole,              QByteArrayLiteral("fileType")},
        {ExtensionRole,         QByteArrayLiteral("extension")},
        {PlayingRole,           QByteArrayLiteral("isPlaying")},
        {IsLocalRole,           QByteArrayLiteral("isLocal")},
        {IsSelectedRole,        QByteArrayLiteral("isSelected")},
        {SectionRole,           QByteArrayLiteral("section")},
        {PlaybackPositionRole,  QByteArrayLiteral("playbackPosition")},
        {TrackNoRole,           QByteArrayLiteral("trackNo")},
        {DiscNoRole,            QByteArrayLiteral("discNo")},
        {TitleRole,             QByteArrayLiteral("title")},
        {ReleaseYearRole,       QByteArrayLiteral("releaseYear")},
        {GenreRole,             QByteArrayLiteral("album")},
        {AlbumRole,             QByteArrayLiteral("album")},
        {ArtistRole,            QByteArrayLiteral("artist")},
        {AlbumArtistRole,       QByteArrayLiteral("albumArtist")},
        {ComposerRole,          QByteArrayLiteral("composer")},
        {LyricistRole,          QByteArrayLiteral("lyricist")},
        {AudioCodecRole,        QByteArrayLiteral("audioCodec")},
        {SampleRateRole,        QByteArrayLiteral("sampleRate")},
        {BitrateRole,           QByteArrayLiteral("bitrate")},
        {DisplayedWidthRole,    QByteArrayLiteral("displayedWidth")},
        {DisplayedHeightRole,   QByteArrayLiteral("displayedHeight")},
        {FramerateRole,         QByteArrayLiteral("frameRate")},
        {VideoCodecRole,        QByteArrayLiteral("videoCodec")},
        {OrientationRole,       QByteArrayLiteral("orientation")},
    };
    // clang-format on

    return roles;
}

void PlaylistModel::clear()
{
    m_threadPool.clear();

    m_playlistPath = QString();
    m_playingItem = 0;
    beginResetModel();
    m_playlist.clear();
    endResetModel();
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
        auto mimeType = MiscUtils::mimeType(url);

        if (mimeType == u"audio/x-mpegurl"_s) {
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

void PlaylistModel::stop()
{
    m_isPlaying = false;
    if (m_playlist.empty()) {
        return;
    }
    m_playlist[m_playingItem].playbackPosition = getPlaybackPosition(m_playingItem);
    Q_EMIT dataChanged(index(m_playingItem, 0), index(m_playingItem, 0));
}

void PlaylistModel::appendItem(const QUrl &url)
{
    PlaylistItem item;
    QFileInfo itemInfo(url.toLocalFile());
    QDir itemDir(itemInfo.absolutePath());
    auto row{m_playlist.size()};
    if (itemInfo.exists() && itemInfo.isFile()) {
        item.url = url;
        item.filename = itemInfo.fileName();
        item.folderPath = itemInfo.absolutePath();
        item.dirName = itemDir.dirName();
        item.modifiedDate = itemInfo.lastModified();
        item.fileSize = itemInfo.size();
        item.extension = itemInfo.suffix();
        item.fileType = MiscUtils::mimeType(url).split(u"/"_s).first();
    } else {
        if (url.scheme().startsWith(u"http"_s)) {
            item.url = url;
            item.filename = url.toString();
            item.fileType = u"http"_s;
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
    Q_EMIT itemAdded(row, item.url.toString(), m_playlistName);

    endInsertRows();

    if (PlaylistSettings::randomPlayback()) {
        shuffleIndexes();
    }
}

void PlaylistModel::removeItem(const uint row)
{
    beginRemoveRows(QModelIndex(), row, row);
    m_playlist.erase(m_playlist.begin() + row);
    endRemoveRows();
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
        QString mimeType = MiscUtils::mimeType(siblingUrl);
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
        QDir itemDir(fileInfo.absolutePath());
        auto fileUrl = QUrl::fromLocalFile(file);
        PlaylistItem item;
        item.url = fileUrl;
        item.filename = fileInfo.fileName();
        item.folderPath = fileInfo.absolutePath();
        item.dirName = itemDir.dirName();
        item.modifiedDate = fileInfo.lastModified();
        item.fileSize = fileInfo.size();
        item.extension = fileInfo.suffix();
        item.fileType = MiscUtils::mimeType(fileUrl).split(u"/"_s).first();
        m_playlist.push_back(item);
        // in flatpak the file dialog gives a percent encoded path
        // use toLocalFile to normalize the urls
        if (url.toLocalFile() == fileUrl.toLocalFile()) {
            playingItem = m_playlist.size() - 1;
        }
        Q_EMIT itemAdded(m_playlist.size() - 1, item.url.toString(), m_playlistName);
    }
    endInsertRows();

    setPlayingItem(playingItem);

    if (PlaylistSettings::randomPlayback()) {
        shuffleIndexes();
    }
}

void PlaylistModel::addM3uItems(const QUrl &url, Behavior behavior)
{
    if (url.scheme() != u"file"_s || MiscUtils::mimeType(url) != u"audio/x-mpegurl"_s) {
        return;
    }

    m_playlistPath = url.toString();
    QFile m3uFile(url.toString(QUrl::PreferLocalFile));
    if (!m3uFile.open(QFile::ReadOnly)) {
        qDebug() << "can't open playlist file";
        return;
    }

    uint playingItem{0};
    int i{0};
    bool matchFound{false};
    while (!m3uFile.atEnd()) {
        QByteArray line = QByteArray::fromPercentEncoding(m3uFile.readLine());
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

    if (PlaylistSettings::randomPlayback()) {
        shuffleIndexes();
    }
}

void PlaylistModel::addYouTubePlaylist(const QJsonArray &playlist, const QString &videoId, const QString &playlistId)
{
    bool matchFound{false};
    for (int i = 0; i < playlist.size(); ++i) {
        const auto _playlist = playlist.at(i).toObject();
        auto id = _playlist.value(u"id"_s).toString();
        auto url = u"https://www.youtube.com/watch?v=%1&list=%2"_s.arg(id, playlistId);
        auto title = _playlist.value(u"title"_s).toString();
        auto duration = _playlist.value(u"duration"_s).toDouble();

        PlaylistItem item;
        item.url = QUrl::fromUserInput(url);
        item.filename = !title.isEmpty() ? title : url;
        item.mediaTitle = !title.isEmpty() ? title : url;
        item.formattedDuration = MiscUtils::formatTime(duration);
        item.duration = duration;

        beginInsertRows(QModelIndex(), m_playlist.size(), m_playlist.size());
        m_playlist.push_back(item);
        Q_EMIT itemAdded(i, item.url.toString(), m_playlistName);
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

void PlaylistModel::updateFileInfo(const YTVideoInfo &info, const QVariantMap &data)
{
    const auto row = data.value(u"row"_s).toUInt();
    const auto item{m_playlist.at(row)};
    if (info.url != item.url) {
        return;
    }

    m_playlist[row].mediaTitle = info.mediaTitle;
    m_playlist[row].filename = info.mediaTitle;
    m_playlist[row].formattedDuration = MiscUtils::formatTime(info.duration);
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
    m_isPlaying = true;

    m_playlist[previousItem].playbackPosition = getPlaybackPosition(previousItem);

    Q_EMIT dataChanged(index(previousItem, 0), index(previousItem, 0));
    Q_EMIT dataChanged(index(i, 0), index(i, 0));
    Q_EMIT playingItemChanged(m_playlistName);

    GeneralSettings::setLastPlayedFile(m_playlist.at(i).url.toString());
    GeneralSettings::setLastPlaylist(m_playlistPath);
    GeneralSettings::self()->save();
}

void PlaylistModel::getMetaData(uint i, const QString &path)
{
    m_threadPool.start([this, i, path]() {
        using namespace KFileMetaData;

        auto url = QUrl::fromUserInput(path);
        if (url.scheme() != u"file"_s) {
            return;
        }

        QString mimeType = MiscUtils::mimeType(url);
        ExtractorCollection exCol;
        QList<Extractor *> extractors = exCol.fetchExtractors(mimeType);
        SimpleExtractionResult result(url.toLocalFile(), mimeType, ExtractionResult::ExtractMetaData);

        if (extractors.isEmpty()) {
            return;
        }

        Extractor *ex = extractors.first();
        ex->extract(&result);

        Q_EMIT metaDataReady(i, url, result.properties());
    });
}

void PlaylistModel::onMetaDataReady(uint i, const QUrl &url, const KFileMetaData::PropertyMultiMap &properties)
{
    if (m_playlist.empty() || static_cast<uint>(i) >= m_playlist.size()) {
        return;
    }

    if (m_playlist.at(i).url == url) {
        auto duration = properties.value(KFileMetaData::Property::Duration).toInt();
        auto title = properties.value(KFileMetaData::Property::Title).toString();

        m_playlist[i].formattedDuration = MiscUtils::formatTime(duration);
        m_playlist[i].mediaTitle = title;
        m_playlist[i].duration = duration;
        m_playlist[i].audio.setMetaData(properties);
        m_playlist[i].video.setMetaData(properties);
        m_playlist[i].playbackPosition = getPlaybackPosition(i);

        Q_EMIT dataChanged(index(i, 0), index(i, 0));
    } else {
        qDebug() << "\n"
                 << u"Data mismatch: the url at position %1 received from the threadpool:"_s.arg(i) << "\n"
                 << u"%1"_s.arg(url.toString()) << "\n"
                 << u"is different than the url in m_playlist at position %2"_s.arg(i) << "\n"
                 << u"%1"_s.arg(m_playlist.at(i).url.toString());
    }
}

double PlaylistModel::getPlaybackPosition(const uint row)
{
    auto duration = m_playlist.at(row).duration;
    auto url = QUrl::fromUserInput(m_playlist.at(row).url.toString());

    if (duration <= 0) {
        return 0.0;
    }

    auto hash = MiscUtils::md5(url.toString());
    return Database::instance()->playbackPosition(hash) / duration;
}

bool PlaylistModel::isPlaying() const
{
    return m_isPlaying;
}

void PlaylistModel::setIsPlaying(bool newIsPlaying)
{
    m_isPlaying = newIsPlaying;
    Q_EMIT dataChanged(index(m_playingItem, 0), index(m_playingItem, 0));
}

void PlaylistModel::shuffleIndexes(const std::vector<int> &includedIndices)
{
    if (m_playlist.size() <= 0) {
        return;
    }
    m_currentShuffledIndex = 0;
    if (includedIndices.empty()) {
        // All indices are included, no filtering
        m_shuffledIndexes.resize(m_playlist.size());
        std::iota(m_shuffledIndexes.begin(), m_shuffledIndexes.end(), 0);
    } else {
        m_shuffledIndexes = includedIndices;
    }
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

#include "moc_playlistmodel.cpp"
