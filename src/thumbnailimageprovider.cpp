/*
 * SPDX-FileCopyrightText: 2021 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "thumbnailimageprovider.h"

#include <QFile>
#include <QIcon>

#include "miscutilities.h"
#include "worker.h"

using namespace Qt::StringLiterals;

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
            if (url.scheme() != u"file"_s) {
                return;
            }
            QString mimeType = MiscUtilities::mimeType(url);
            QString iconName;
            if (mimeType.startsWith(u"video/"_s)) {
                iconName = u"video-x-generic"_s;
            } else if (mimeType.startsWith(u"audio/"_s)) {
                iconName = u"audio-x-generic"_s;
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

    if (url.scheme() == u"http"_s || url.scheme() == u"https"_s) {
        auto icon = QIcon::fromTheme(u"im-youtube"_s, QIcon::fromTheme(u"video-x-generic"_s));
        m_texture = QQuickTextureFactory::textureFactoryForImage(icon.pixmap(requestedSize).toImage());
        Q_EMIT finished();
    }
}

QQuickTextureFactory *ThumbnailResponse::textureFactory() const
{
    return m_texture;
}
