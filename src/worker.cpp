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
#include <KWindowConfig>

#include "application.h"
#include "framedecoder.h"
#include "subtitlessettings.h"

using namespace Qt::StringLiterals;

Worker *Worker::instance()
{
    static Worker w;
    return &w;
}

void Worker::getMetaData(uint index, const QString &path)
{
    using namespace KFileMetaData;

    auto url = QUrl::fromUserInput(path);
    if (url.scheme() != u"file"_s) {
        return;
    }
    QString mimeType = Application::mimeType(url);
    ExtractorCollection exCol;
    QList<Extractor *> extractors = exCol.fetchExtractors(mimeType);
    SimpleExtractionResult result(url.toLocalFile(), mimeType, ExtractionResult::ExtractMetaData);
    if (extractors.size() == 0) {
        return;
    }
    Extractor *ex = extractors.first();
    ex->extract(&result);

    auto properties = result.properties();

    Q_EMIT metaDataReady(index, url, properties);
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

void Worker::findRecursiveSubtitles(const QUrl &url)
{
    QStringList subs;
    auto parentFolder = QFileInfo(url.toLocalFile()).absolutePath();
    const auto subFolders = SubtitlesSettings::self()->subtitlesFolders();
    for (const auto &subFolder : subFolders) {
        if (subFolder.startsWith(u"/"_s)) {
            // ignore absolute paths
            continue;
        }

        auto searchFolder = QString(parentFolder).append(u"/%1"_s.arg(subFolder));

        QFileInfo fi(searchFolder);
        if (!fi.exists()) {
            continue;
        }

        QDirIterator it{searchFolder, QDir::Files, QDirIterator::Subdirectories};
        while (it.hasNext()) {
            auto fi = it.nextFileInfo();

            auto url = QUrl::fromLocalFile(fi.absoluteFilePath());
            QString mimeType = Application::mimeType(url);
            if (mimeType.startsWith(u"application/x-subrip"_s) || mimeType.startsWith(u"text/x-ssa"_s)) {
                auto currentFI = QFileInfo(url.toLocalFile());
                // The subtitles path must contain the name of the playing file
                if (fi.absoluteFilePath().contains(currentFI.baseName())) {
                    subs.append(fi.absoluteFilePath());
                }
            }
        }
    }

    if (subs.isEmpty()) {
        return;
    }

    Q_EMIT subtitlesFound(subs);
}

void Worker::syncConfigValue(QString path, QString group, QString key, QVariant value)
{
    if (!m_cachedConf || m_cachedConf->name() != path) {
        m_cachedConf.reset(new KConfig(path));
    }

    m_cachedConf->group(group).writeEntry(key, value);
    m_cachedConf->sync();
}

void Worker::getYtdlpVersion()
{
    QProcess ytdlpProcess;
    ytdlpProcess.setProgram(Application::youtubeDlExecutable());
    ytdlpProcess.setArguments({u"--version"_s});
    ytdlpProcess.start();
    ytdlpProcess.waitForFinished();
    auto ytdlpVersion = ytdlpProcess.readAllStandardOutput().simplified();

    Q_EMIT ytdlpVersionRetrived(ytdlpVersion);
}

#include "moc_worker.cpp"
