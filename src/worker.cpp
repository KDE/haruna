/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "worker.h"

#include <QCryptographicHash>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QImage>
#include <QProcess>
#include <QQuickWindow>
#include <QThread>

#include <KConfig>
#include <KConfigGroup>
#include <KFileMetaData/ExtractorCollection>
#include <KFileMetaData/SimpleExtractionResult>

#include "miscutilities.h"
#include "database.h"
#include "framedecoder.h"
#include "pathutils.h"
#include "subtitlessettings.h"
#include "youtube.h"

using namespace Qt::StringLiterals;

Worker *Worker::instance()
{
    static Worker w;
    return &w;
}

void Worker::getMetaData(uint index, const QString &path, const QString playlistName)
{
    using namespace KFileMetaData;

    auto url = QUrl::fromUserInput(path);
    if (url.scheme() != u"file"_s) {
        return;
    }
    QString mimeType = MiscUtilities::mimeType(url);
    ExtractorCollection exCol;
    QList<Extractor *> extractors = exCol.fetchExtractors(mimeType);
    SimpleExtractionResult result(url.toLocalFile(), mimeType, ExtractionResult::ExtractMetaData);
    if (extractors.size() == 0) {
        return;
    }
    Extractor *ex = extractors.first();
    ex->extract(&result);

    auto properties = result.properties();

    Q_EMIT metaDataReady(index, url, playlistName, properties);
}

void Worker::makePlaylistThumbnail(const QString &path, int width)
{
    QImage image;

    auto file = QUrl::fromUserInput(path);

    // figure out absolute path of the thumbnail
    auto md5Hash = QCryptographicHash::hash(file.toString().toUtf8(), QCryptographicHash::Md5);
    QString cacheDir(QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation));
    QString appDir(u"haruna"_s);
    QString fileDir(QString::fromUtf8(md5Hash.toHex()));
    QString filename(QString::fromUtf8(md5Hash.toHex()).append(u".png"_s));
    QString cachedFilePath = u"%1/%2/%3/%4"_s.arg(cacheDir, appDir, fileDir, filename);

    // load existing thumbnail if there is one
    if (QFileInfo::exists(cachedFilePath) && image.load(cachedFilePath)) {
        Q_EMIT thumbnailSuccess(path, image);
        return;
    }

    image = frameToImage(path, width);

    if (image.isNull()) {
        qDebug() << u"Failed to create thumbnail for file: %1"_s.arg(path);
        return;
    }
    Q_EMIT thumbnailSuccess(path, image);

    QFileInfo fi(cachedFilePath);
    // create folders where the file will be saved
    if (QDir().mkpath(fi.absolutePath())) {
        if (!image.save(cachedFilePath)) {
            qDebug() << u"Failed to save thumbnail for file: %1"_s.arg(path);
        }
    }
}

QImage Worker::frameToImage(const QString &path, int width)
{
    QImage image;
    FrameDecoder frameDecoder(path, nullptr);
    if (!frameDecoder.getInitialized()) {
        return image;
    }
    // before seeking, a frame has to be decoded
    if (!frameDecoder.decodeVideoFrame()) {
        return image;
    }

    int secondToSeekTo = frameDecoder.getDuration() * 20 / 100;
    frameDecoder.seek(secondToSeekTo);
    frameDecoder.getScaledVideoFrame(width, true, image);

    return image;
}

void Worker::mprisThumbnail(const QString &path, int width)
{
    QImage image;
    FrameDecoder frameDecoder(path, nullptr);
    if (!frameDecoder.getInitialized()) {
        return;
    }
    // before seeking, a frame has to be decoded
    if (!frameDecoder.decodeVideoFrame()) {
        return;
    }

    int secondToSeekTo = frameDecoder.getDuration() * 20 / 100;
    frameDecoder.seek(secondToSeekTo);
    frameDecoder.getScaledVideoFrame(width, true, image);

    Q_EMIT mprisThumbnailSuccess(image);
}

void Worker::findRecursiveSubtitles(const QUrl &playingUrl)
{
    const auto playingFileInfo = QFileInfo(playingUrl.toLocalFile());
    const auto parentFolder = playingFileInfo.absolutePath();

    QStringList searchFolders;
    QDirIterator it{parentFolder, QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::NoIteratorFlags};
    while (it.hasNext()) {
        auto folder = it.nextFileInfo();
        const auto subFolders = SubtitlesSettings::subtitlesFolders();
        for (const auto &sf : subFolders) {
            if (sf.startsWith(u"/"_s)) {
                // ignore absolute paths
                continue;
            }

            if (folder.fileName() == sf) {
                // exact match
                searchFolders.append(folder.absoluteFilePath());
                continue;
            }

            QString _sf{sf};
            if (sf.startsWith(u"*"_s)) {
                _sf = _sf.removeFirst();
                if (folder.fileName().contains(_sf, Qt::CaseInsensitive)) {
                    searchFolders.append(folder.absoluteFilePath());
                }
            }
        }
    }

    QStringList foundSubs;
    for (const auto &searchFolder : searchFolders) {
        QDirIterator it{searchFolder, QDir::Files, QDirIterator::Subdirectories};
        while (it.hasNext()) {
            auto fi = it.nextFileInfo();
            auto url = QUrl::fromLocalFile(fi.absoluteFilePath());
            QString mimeType = MiscUtilities::mimeType(url);
            if (mimeType.startsWith(u"application/x-subrip"_s) || mimeType.startsWith(u"text/x-ssa"_s)) {
                // The subtitles path must contain the name of the playing file
                if (fi.absoluteFilePath().contains(playingFileInfo.completeBaseName(), Qt::CaseInsensitive)) {
                    foundSubs.append(fi.absoluteFilePath());
                }
            }
        }
    }

    if (foundSubs.isEmpty()) {
        return;
    }

    Q_EMIT subtitlesFound(foundSubs);
}

void Worker::savePositionToDB(const QString &md5Hash, const QString &path, double position)
{
    Database::instance()->addPlaybackPosition(md5Hash, path, position, getDBConnection());
}

void Worker::getYtdlpVersion()
{
    QProcess ytdlpProcess;
    YouTube yt;
    ytdlpProcess.setProgram(yt.youtubeDlExecutable());
    ytdlpProcess.setArguments({u"--version"_s});
    ytdlpProcess.start();
    ytdlpProcess.waitForFinished();
    auto ytdlpVersion = ytdlpProcess.readAllStandardOutput().simplified();

    Q_EMIT ytdlpVersionRetrived(ytdlpVersion);
}

QSqlDatabase Worker::getDBConnection()
{
    static const auto dbFile{PathUtils::instance()->configFilePath(PathUtils::ConfigFile::Database)};
    static auto db = QSqlDatabase::addDatabase(u"QSQLITE"_s, u"worker_connection"_s);
    if (!db.isOpen()) {
        db.setDatabaseName(dbFile);
        db.open();
    }

    return db;
}
#include "moc_worker.cpp"
