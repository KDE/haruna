/*
 * SPDX-FileCopyrightText: 2025 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef MISCUTILS_H
#define MISCUTILS_H

#include <QObject>
#include <QQmlEngine>

#include <KFileMetaData/SimpleExtractionResult>

struct Metadata {
    qint64 metadataId = 0;
    QUrl url;
    KFileMetaData::PropertyMultiMap properties;
    QMap<KFileMetaData::EmbeddedImageData::ImageType, QByteArray> imageData;
};

class MiscUtils : public QObject
{
    Q_OBJECT
    QML_SINGLETON
    QML_ELEMENT

public:
    static MiscUtils *instance();
    static MiscUtils *create(QQmlEngine *, QJSEngine *);

    Q_INVOKABLE static QString formatTime(const double time);
    Q_INVOKABLE static QString mimeType(const QUrl &url);
    Q_INVOKABLE static QString md5(const QString &str);
    Q_INVOKABLE static std::optional<Metadata> metadata(const QUrl &url);

Q_SIGNALS:
    void error(const QString &message);

private:
    MiscUtils();
    ~MiscUtils() = default;

    MiscUtils(const MiscUtils &) = delete;
    MiscUtils &operator=(const MiscUtils &) = delete;
    MiscUtils(MiscUtils &&) = delete;
    MiscUtils &operator=(MiscUtils &&) = delete;
};

#endif // MISCUTILS_H
