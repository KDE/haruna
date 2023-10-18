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
        [=](const QString &path, const QImage &image) {
            if (path == QUrl::fromUserInput(id).toLocalFile()) {
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
            auto url = QUrl::fromUserInput(id);
            if (url.scheme() != QStringLiteral("file")) {
                return;
            }
            QString mimeType = Application::mimeType(url);
            QString iconName;
            if (mimeType.startsWith(QStringLiteral("video/"))) {
                iconName = QStringLiteral("video-x-generic");
            } else if (mimeType.startsWith(QStringLiteral("audio/"))) {
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
    auto url = QUrl::fromUserInput(id);
    if (QFile(url.toLocalFile()).exists()) {
        Worker::instance()->makePlaylistThumbnail(url.toLocalFile(), requestedSize.width());
    }

    if (url.scheme() == QStringLiteral("http") || url.scheme() == QStringLiteral("https")) {
        auto icon = QIcon::fromTheme(QStringLiteral("im-youtube"), QIcon::fromTheme(QStringLiteral("video-x-generic")));
        m_texture = QQuickTextureFactory::textureFactoryForImage(icon.pixmap(requestedSize).toImage());
        Q_EMIT finished();
    }
}

QQuickTextureFactory *ThumbnailResponse::textureFactory() const
{
    return m_texture;
}
