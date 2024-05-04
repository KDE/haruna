/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "worker.h"

#include "application.h"
#include "framedecoder.h"
#include "generalsettings.h"

#include <QCryptographicHash>
#include <QDir>
#include <QFileInfo>
#include <QImage>
#include <QQuickWindow>
#include <QThread>

#include <KConfig>
#include <KConfigGroup>
#include <KFileMetaData/ExtractorCollection>
#include <KFileMetaData/SimpleExtractionResult>
#include <KWindowConfig>

Worker *Worker::instance()
{
    static Worker w;
    return &w;
}

void Worker::getMetaData(int index, const QString &path)
{
    using namespace KFileMetaData;

    auto url = QUrl::fromUserInput(path);
    if (url.scheme() != QStringLiteral("file")) {
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

    Q_EMIT metaDataReady(index, properties);
}

void Worker::makePlaylistThumbnail(const QString &path, int width)
{
    QImage image;

    auto file = QUrl::fromUserInput(path);

    // figure out absolute path of the thumbnail
    auto md5Hash = QCryptographicHash::hash(file.toString().toUtf8(), QCryptographicHash::Md5);
    QString cacheDir(QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation));
    QString appDir(QStringLiteral("haruna"));
    QString fileDir(QString::fromUtf8(md5Hash.toHex()));
    QString filename(QString::fromUtf8(md5Hash.toHex()).append(QStringLiteral(".png")));
    QString separator(QStringLiteral("/"));
    QString cachedFilePath = cacheDir + separator + appDir + separator + fileDir + separator + filename;

    // load existing thumbnail if there is one
    if (QFileInfo::exists(cachedFilePath) && image.load(cachedFilePath)) {
        Q_EMIT thumbnailSuccess(path, image);
        return;
    }

    image = frameToImage(path, width);

    if (image.isNull()) {
        qDebug() << QStringLiteral("Failed to create thumbnail for file: %1").arg(path);
        return;
    }
    Q_EMIT thumbnailSuccess(path, image);

    QFileInfo fi(cachedFilePath);
    // create folders where the file will be saved
    if (QDir().mkpath(fi.absolutePath())) {
        if (!image.save(cachedFilePath)) {
            qDebug() << QStringLiteral("Failed to save thumbnail for file: %1").arg(path);
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

void Worker::syncConfigValue(QString path, QString group, QString key, QVariant value)
{
    if (!m_cachedConf || m_cachedConf->name() != path) {
        m_cachedConf.reset(new KConfig(path));
    }

    m_cachedConf->group(group).writeEntry(key, value);
    m_cachedConf->sync();
}

void Worker::saveWindowGeometry(QQuickWindow *window) const
{
    if (!GeneralSettings::rememberWindowGeometry()) {
        return;
    }
    KConfig dataResource(QStringLiteral("data"), KConfig::SimpleConfig, QStandardPaths::AppDataLocation);
    KConfigGroup windowGroup(&dataResource, QStringLiteral("Window"));
    KWindowConfig::saveWindowPosition(window, windowGroup);
    KWindowConfig::saveWindowSize(window, windowGroup);
    dataResource.sync();
}

#include "moc_worker.cpp"
