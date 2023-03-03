/*
 * SPDX-FileCopyrightText: 2021 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "thumbnailimageprovider.h"

#include "application.h"
#include "worker.h"

#include <QFile>
#include <QIcon>

ThumbnailImageProvider::ThumbnailImageProvider()
{
}

QQuickImageResponse *ThumbnailImageProvider::requestImageResponse(const QString &id, const QSize &requestedSize)
{
    auto response = new ThumbnailResponse(id, requestedSize);
    return response;
}

ThumbnailResponse::ThumbnailResponse(const QString &id, const QSize &requestedSize)
{
    connect(
        Worker::instance(),
        &Worker::thumbnailSuccess,
        this,
        [=](const QString &resultId, const QImage &image) {
            if (resultId == id) {
                m_texture = QQuickTextureFactory::textureFactoryForImage(image);
                Q_EMIT finished();
            }
        },
        Qt::QueuedConnection);

    connect(
        Worker::instance(),
        &Worker::thumbnailFail,
        this,
        [=]() {
            QString mimeType = Application::mimeType(id);
            QString iconName;
            if (mimeType.startsWith("video/")) {
                iconName = QStringLiteral("video-x-generic");
            } else if (mimeType.startsWith("audio/")) {
                iconName = QStringLiteral("audio-x-generic");
            } else {
                return;
            }
            auto icon = QIcon::fromTheme(iconName).pixmap(requestedSize);
            m_texture = QQuickTextureFactory::textureFactoryForImage(icon.toImage());

            Q_EMIT finished();
        },
        Qt::QueuedConnection);
    getPreview(id, requestedSize);
}

void ThumbnailResponse::getPreview(const QString &id, const QSize &requestedSize)
{
    if (QFile(id).exists()) {
        Worker::instance()->makePlaylistThumbnail(id, requestedSize.width());
    }

    if (QUrl(id).scheme() == "http" || QUrl(id).scheme() == "https") {
        auto icon = QIcon::fromTheme("im-youtube", QIcon::fromTheme("video-x-generic"));
        m_texture = QQuickTextureFactory::textureFactoryForImage(icon.pixmap(requestedSize).toImage());
        Q_EMIT finished();
    }
}

QQuickTextureFactory *ThumbnailResponse::textureFactory() const
{
    return m_texture;
}
