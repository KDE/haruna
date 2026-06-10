/*
 * SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "mpvpreview.h"

#include <QMimeDatabase>

#include <MpvController>
#include <playbacksettings.h>

#include "generalsettings.h"
#include "mpvproperties.h"

using namespace Qt::StringLiterals;

MpvPreview::MpvPreview()
{
    setProperty(MpvProperties::self()->VO, u"libmpv"_s);
    observeProperty(MpvProperties::self()->Position, MPV_FORMAT_DOUBLE);
    observeProperty(MpvProperties::self()->AspectRatio, MPV_FORMAT_DOUBLE);

    setProperty(MpvProperties::self()->Mute, true);
    setProperty(MpvProperties::self()->Pause, true);
    setProperty(MpvProperties::self()->ReallyQuiet, true);

    setProperty(MpvProperties::self()->HardwareDecoding, PlaybackSettings::hWDecoding());
    setProperty(MpvProperties::self()->AccurateSeek, GeneralSettings::accuratePreviewThumbnail());
    setProperty(MpvProperties::self()->AudioId, false);
    setProperty(MpvProperties::self()->AudioFileAuto, false);
    setProperty(MpvProperties::self()->SubtitleId, false);
    setProperty(MpvProperties::self()->SubtitleAuto, false);
    setProperty(MpvProperties::self()->OsdLevel, 0);
    setProperty(MpvProperties::self()->AudioPitchCorection, false);
    setProperty(MpvProperties::self()->UseTextOsd, false);
    setProperty(MpvProperties::self()->AudioDisplay, false);

    connect(mpvController(), &MpvController::propertyChanged, this, [this](const QString &property, const QVariant &value) {
        Q_UNUSED(value)
        if (property == MpvProperties::self()->AspectRatio) {
            Q_EMIT aspectRatioChanged();
        }
    });
    connect(this, &MpvPreview::fileChanged, this, &MpvPreview::loadFile);
    connect(this, &MpvPreview::ready, this, [this]() {
        m_isReady = true;
        loadFile();
    });
}

void MpvPreview::loadFile()
{
    if (m_isReady && !m_file.isEmpty()) {
        command(QStringList() << u"loadfile"_s << m_file);
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
    setProperty(MpvProperties::self()->AccurateSeek, _accuratePreview);
    Q_EMIT accuratePreviewChanged();
}

#include "moc_mpvpreview.cpp"
