/*
 * SPDX-FileCopyrightText: 2021 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef THUMBNAILIMAGEPROVIDER_H
#define THUMBNAILIMAGEPROVIDER_H

#include <QQuickAsyncImageProvider>

class ThumbnailImageProvider : public QQuickAsyncImageProvider
{
public:
    explicit ThumbnailImageProvider();
    QQuickImageResponse *requestImageResponse(const QString &id, const QSize &requestedSize) override;
};

class ThumbnailResponse : public QQuickImageResponse
{
public:
    ThumbnailResponse(const QString &id, const QSize &requestedSize);

    QQuickTextureFactory *textureFactory() const override;
    void getPreview(const QString &id, const QSize &requestedSize);

    QQuickTextureFactory *m_texture{nullptr};
};

#endif // THUMBNAILIMAGEPROVIDER_H
