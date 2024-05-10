/*
 * SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "mpvpreview.h"

#include <MpvController>
#include <QMimeDatabase>

#include "generalsettings.h"
#include "mpvproperties.h"

MpvPreview::MpvPreview()
{
    Q_EMIT setProperty(MpvProperties::self()->VO, QStringLiteral("libmpv"));
    Q_EMIT observeProperty(MpvProperties::self()->Position, MPV_FORMAT_DOUBLE);
    Q_EMIT observeProperty(MpvProperties::self()->AspectRatio, MPV_FORMAT_DOUBLE);

    Q_EMIT setProperty(MpvProperties::self()->Mute, true);
    Q_EMIT setProperty(MpvProperties::self()->Pause, true);
    Q_EMIT setProperty(MpvProperties::self()->ReallyQuiet, true);

    Q_EMIT setProperty(MpvProperties::self()->HardwareDecoding, QStringLiteral("auto-safe"));
    Q_EMIT setProperty(MpvProperties::self()->AccurateSeek, GeneralSettings::accuratePreviewThumbnail());
    Q_EMIT setProperty(MpvProperties::self()->AudioId, false);
    Q_EMIT setProperty(MpvProperties::self()->AudioFileAuto, false);
    Q_EMIT setProperty(MpvProperties::self()->SubtitleId, false);
    Q_EMIT setProperty(MpvProperties::self()->SubtitleAuto, false);
    Q_EMIT setProperty(MpvProperties::self()->OsdLevel, 0);
    Q_EMIT setProperty(MpvProperties::self()->AudioPitchCorection, false);
    Q_EMIT setProperty(MpvProperties::self()->UseTextOsd, false);
    Q_EMIT setProperty(MpvProperties::self()->AudioDisplay, false);

    connect(mpvController(), &MpvController::propertyChanged, this, [=](const QString &property, const QVariant &value) {
        Q_UNUSED(value)
        if (property == MpvProperties::self()->AspectRatio) {
            Q_EMIT aspectRatioChanged();
        }
    });
    connect(this, &MpvPreview::fileChanged, this, &MpvPreview::loadFile);
    connect(this, &MpvPreview::ready, this, [=]() {
        m_isReady = true;
        loadFile();
    });
}

void MpvPreview::loadFile()
{
    if (m_isReady && m_isVideo && !m_file.isEmpty()) {
        Q_EMIT command(QStringList() << QStringLiteral("loadfile") << m_file);
    }
}

double MpvPreview::position()
{
    return getProperty(MpvProperties::self()->Position).toDouble();
}

void MpvPreview::setPosition(double value)
{
    if (qFuzzyCompare(m_position, value)) {
        return;
    }
    setPropertyAsync(MpvProperties::self()->Position, value);
}

double MpvPreview::aspectRatio()
{
    return getProperty(MpvProperties::self()->AspectRatio).toDouble();
}

QString MpvPreview::file() const
{
    return m_file;
}

void MpvPreview::setFile(const QString &_file)
{
    if (m_file == _file) {
        return;
    }
    m_file = _file;
    auto url = QUrl::fromUserInput(m_file);
    setIsLocalFile(url.isLocalFile());

    QMimeDatabase mimeDb;
    QString mimeType = mimeDb.mimeTypeForFile(m_file).name();
    setIsVideo(mimeType.startsWith(u"video/"_qs));

    Q_EMIT fileChanged();
}

bool MpvPreview::accuratePreview() const
{
    return m_accuratePreview;
}

void MpvPreview::setAccuratePreview(bool _accuratePreview)
{
    if (m_accuratePreview == _accuratePreview) {
        return;
    }
    m_accuratePreview = _accuratePreview;
    Q_EMIT setProperty(MpvProperties::self()->AccurateSeek, _accuratePreview);
    Q_EMIT accuratePreviewChanged();
}

bool MpvPreview::isLocalFile() const
{
    return m_isLocalFile;
}

void MpvPreview::setIsLocalFile(bool _isLocalFile)
{
    if (m_isLocalFile == _isLocalFile) {
        return;
    }
    m_isLocalFile = _isLocalFile;
    Q_EMIT isLocalFileChanged();
}

#include "moc_mpvpreview.cpp"

bool MpvPreview::isVideo() const
{
    return m_isVideo;
}

void MpvPreview::setIsVideo(bool _isVideo)
{
    if (m_isVideo == _isVideo) {
        return;
    }
    m_isVideo = _isVideo;
    Q_EMIT isVideoChanged();
}
