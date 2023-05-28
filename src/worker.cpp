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
#include <QDebug>
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
    auto url = QUrl::fromUserInput(path);
    if (url.scheme() != QStringLiteral("file")) {
        return;
    }
    QString mimeType = Application::mimeType(url);
    KFileMetaData::ExtractorCollection exCol;
    QList<KFileMetaData::Extractor *> extractors = exCol.fetchExtractors(mimeType);
    KFileMetaData::SimpleExtractionResult result(path, mimeType, KFileMetaData::ExtractionResult::ExtractMetaData);
    if (extractors.size() == 0) {
        return;
    }
    KFileMetaData::Extractor *ex = extractors.first();
    ex->extract(&result);

    auto properties = result.properties(KFileMetaData::PropertiesMapType::MultiMap);

    Q_EMIT metaDataReady(index, properties);
}

void Worker::makePlaylistThumbnail(const QString &id, int width)
{
    QImage image;

    QUrl file(id);
    file.setScheme(QStringLiteral("file"));

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
        Q_EMIT thumbnailSuccess(id, image);
        return;
    }

    image = frameToImage(id, width);

    if (image.isNull()) {
        qDebug() << QStringLiteral("Failed to create thumbnail for file: %1").arg(id);
        return;
    }
    Q_EMIT thumbnailSuccess(id, image);

    QFileInfo fi(cachedFilePath);
    // create folders where the file will be saved
    if (QDir().mkpath(fi.absolutePath())) {
        if (!image.save(cachedFilePath)) {
            qDebug() << QStringLiteral("Failed to save thumbnail for file: %1").arg(id);
        }
    }
}

QImage Worker::frameToImage(const QString &id, int width)
{
    QImage image;
    FrameDecoder frameDecoder(id, nullptr);
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
