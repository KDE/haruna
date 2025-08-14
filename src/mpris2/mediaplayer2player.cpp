/*
 * SPDX-FileCopyrightText: 2021 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "mediaplayer2player.h"

#include <QBuffer>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusObjectPath>
#include <QImage>

#include <KFileMetaData/ExtractorCollection>
#include <KFileMetaData/SimpleExtractionResult>

#include "application.h"
#include "mpvitem.h"
#include "mpvproperties.h"
#include "videosettings.h"
#include "worker.h"

using namespace Qt::StringLiterals;

MediaPlayer2Player::MediaPlayer2Player(QObject *parent)
    : QDBusAbstractAdaptor(parent)
    , m_mpv{static_cast<MpvItem *>(parent)}
{
    connect(m_mpv, &MpvItem::fileLoaded, this, [=]() {
        if (m_mpv->currentUrl().isLocalFile()) {
            Q_EMIT requestMprisThumbnail(m_mpv->currentUrl().toLocalFile(), 250);
        }
    });

    connect(this, &MediaPlayer2Player::requestMprisThumbnail, Worker::instance(), &Worker::mprisThumbnail);

    connect(Worker::instance(), &Worker::mprisThumbnailSuccess, this, [=](const QImage &image) {
        m_image = image;
        propertiesChanged(u"Metadata"_s, Metadata());
        Q_EMIT metadataChanged();
    });

    connect(m_mpv, &MpvItem::mediaTitleChanged, this, [=]() {
        auto title = m_mpv->mediaTitle();
        if (title.isEmpty() || title.startsWith(u"watch"_s)) {
            return;
        }
        m_image = QImage();
        propertiesChanged(u"Metadata"_s, Metadata());
        Q_EMIT metadataChanged();
    });

    connect(m_mpv, &MpvItem::durationChanged, this, [=]() {
        m_metadata.insert(u"mpris:length"_s, m_mpv->duration() * 1000 * 1000);
        propertiesChanged(u"Metadata"_s, m_metadata);
    });

    connect(m_mpv, &MpvItem::pauseChanged, this, [=]() {
        propertiesChanged(u"PlaybackStatus"_s, PlaybackStatus());
        Q_EMIT playbackStatusChanged();
    });

    connect(m_mpv, &MpvItem::volumeChanged, this, [=]() {
        propertiesChanged(u"Volume"_s, Volume());
        Q_EMIT volumeChanged();
    });
}

void MediaPlayer2Player::propertiesChanged(const QString &property, const QVariant &value)
{
    QVariantMap properties;
    properties[property] = value;

    QDBusMessage msg = QDBusMessage::createSignal(u"/org/mpris/MediaPlayer2"_s, u"org.freedesktop.DBus.Properties"_s, u"PropertiesChanged"_s);
    msg << u"org.mpris.MediaPlayer2.Player"_s << properties << QStringList();

    QDBusConnection::sessionBus().send(msg);
}

QVariantMap MediaPlayer2Player::Metadata()
{
    auto url = m_mpv->currentUrl();
    QString mimeType = Application::mimeType(url);

    m_metadata.insert(u"xesam:title"_s, m_mpv->mediaTitle());
    m_metadata.insert(u"mpris:trackid"_s, QVariant::fromValue<QDBusObjectPath>(QDBusObjectPath(u"/org/kde/haruna"_s)));
    if (mimeType.startsWith(u"audio"_s)) {
        m_metadata.insert(u"mpris:artUrl"_s, getThumbnail(url.toLocalFile()));
    } else {
        m_metadata.insert(u"mpris:artUrl"_s, QString{});
    }
    m_metadata.insert(u"xesam:url"_s, url.toString());

    return m_metadata;
}

QString MediaPlayer2Player::getThumbnail(const QString &path)
{
    auto url = QUrl::fromUserInput(path);
    if (!url.isLocalFile()) {
        return QString();
    }

    QString mimeType = Application::mimeType(url);
    KFileMetaData::ExtractorCollection exCol;
    QList<KFileMetaData::Extractor *> extractors = exCol.fetchExtractors(mimeType);
    KFileMetaData::SimpleExtractionResult result(path, mimeType, KFileMetaData::ExtractionResult::ExtractImageData);

    if (extractors.size() > 0) {
        KFileMetaData::Extractor *ex = extractors.first();
        ex->extract(&result);
        QString base64 = u"data:image/png;base64,"_s;
        auto imageData = result.imageData();
        if (!imageData.isEmpty()) {
            // look for image inside the file
            using namespace KFileMetaData;
            if (imageData.contains(EmbeddedImageData::MovieScreenCapture)) {
                return base64.append(QString::fromUtf8(imageData[EmbeddedImageData::MovieScreenCapture].toBase64()));
            } else if (imageData.contains(EmbeddedImageData::FrontCover)) {
                return base64.append(QString::fromUtf8(imageData[EmbeddedImageData::FrontCover].toBase64()));
            } else if (imageData.contains(EmbeddedImageData::BackCover)) {
                return base64.append(QString::fromUtf8(imageData[EmbeddedImageData::BackCover].toBase64()));
            } else if (imageData.contains(EmbeddedImageData::Other)) {
                return base64.append(QString::fromUtf8(imageData[EmbeddedImageData::Other].toBase64()));
            } else {
                return base64.append(QString::fromUtf8(imageData[EmbeddedImageData::Unknown].toBase64()));
            }
        } else {
            QImage image = m_image;
            if (image.isNull()) {
                return VideoSettings::defaultCover();
            } else {
                QByteArray byteArray;
                QBuffer buffer(&byteArray);
                image.save(&buffer, "JPEG");
                return base64.append(QString::fromUtf8(byteArray.toBase64().data()));
            }
        }
    }
    return QString();
}

void MediaPlayer2Player::Next()
{
    Q_EMIT next();
}

void MediaPlayer2Player::Previous()
{
    Q_EMIT previous();
}

void MediaPlayer2Player::Pause()
{
    Q_EMIT pause();
}

void MediaPlayer2Player::PlayPause()
{
    Q_EMIT playpause();
}

void MediaPlayer2Player::Stop()
{
    Q_EMIT stop();
}

void MediaPlayer2Player::Play()
{
    Q_EMIT play();
}

void MediaPlayer2Player::Seek(qlonglong offset)
{
    Q_EMIT seek(offset / 1000 / 1000);
}

void MediaPlayer2Player::SetPosition(const QDBusObjectPath &trackId, qlonglong pos)
{
    Q_UNUSED(trackId)
    Q_EMIT m_mpv->setProperty(MpvProperties::self()->Position, pos / 1000 / 1000);
}

void MediaPlayer2Player::OpenUri(const QString &uri)
{
    Q_EMIT openUri(uri);
}

QString MediaPlayer2Player::PlaybackStatus()
{
    bool isPaused = m_mpv->pause();
    double position = m_mpv->position();

    return isPaused && position == 0 ? u"Stopped"_s : (isPaused ? u"Paused"_s : u"Playing"_s);
}

double MediaPlayer2Player::Volume()
{
    return m_mpv->getProperty(MpvProperties::self()->Volume).toDouble() / 100;
}

void MediaPlayer2Player::setVolume(double vol)
{
    Q_EMIT m_mpv->setProperty(MpvProperties::self()->Volume, vol * 100);
}

qlonglong MediaPlayer2Player::Position()
{
    return m_mpv->position() * 1000 * 1000;
}

void MediaPlayer2Player::setPosition(int pos)
{
    Q_EMIT m_mpv->setProperty(MpvProperties::self()->Position, pos);
}

bool MediaPlayer2Player::CanGoNext()
{
    return true;
}

bool MediaPlayer2Player::CanGoPrevious()
{
    return true;
}

bool MediaPlayer2Player::CanPlay()
{
    return true;
}

bool MediaPlayer2Player::CanPause()
{
    return true;
}

bool MediaPlayer2Player::CanSeek()
{
    return true;
}

bool MediaPlayer2Player::CanControl()
{
    return true;
}

#include "moc_mediaplayer2player.cpp"
