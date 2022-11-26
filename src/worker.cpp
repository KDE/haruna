/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "worker.h"

#include "application.h"
#include "framedecoder.h"

#include <QCryptographicHash>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QImage>
#include <QThread>

#include <KConfig>
#include <KConfigGroup>
#include <KFileMetaData/ExtractorCollection>
#include <KFileMetaData/SimpleExtractionResult>

Worker* Worker::instance()
{
    static Worker w;
    return &w;
}

void Worker::getMetaData(int index, const QString &path)
{
    QString mimeType = Application::mimeType(QUrl(path));
    KFileMetaData::ExtractorCollection exCol;
    QList<KFileMetaData::Extractor*> extractors = exCol.fetchExtractors(mimeType);
    KFileMetaData::SimpleExtractionResult result(path, mimeType,
                                                 KFileMetaData::ExtractionResult::ExtractMetaData);
    if (extractors.size() == 0) {
        return;
    }
    KFileMetaData::Extractor* ex = extractors.first();
    ex->extract(&result);
#if KCONFIG_VERSION >= QT_VERSION_CHECK(5, 89, 0)
    auto properties = result.properties(KFileMetaData::MultiMap);
#else
    auto properties = result.properties();
#endif

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
    QString appDir("haruna");
    QString fileDir(md5Hash.toHex());
    QString filename(QString(md5Hash.toHex()).append(".png"));
    QString cachedFilePath = cacheDir + "/" + appDir + "/" + fileDir + "/" + filename;

    // load existing thumbnail if there is one
    if (QFileInfo::exists(cachedFilePath) && image.load(cachedFilePath)) {
        Q_EMIT thumbnailSuccess(id, image);
        return;
    }

    FrameDecoder frameDecoder(file.toLocalFile(), nullptr);
    if (!frameDecoder.getInitialized()) {
        return;
    }
    //before seeking, a frame has to be decoded
    if (!frameDecoder.decodeVideoFrame()) {
        return;
    }

    int secondToSeekTo = frameDecoder.getDuration() * 20 / 100;
    frameDecoder.seek(secondToSeekTo);

    frameDecoder.getScaledVideoFrame(width, true, image);

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

void Worker::syncConfigValue(QString path, QString group, QString key, QVariant value)
{
    if (!m_cachedConf || m_cachedConf->name() != path) {
        m_cachedConf.reset(new KConfig(path));
    }

    m_cachedConf->group(group).writeEntry(key, value);
    m_cachedConf->sync();
}
