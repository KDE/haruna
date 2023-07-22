/*
 * SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "mpvpreview.h"

#include "generalsettings.h"

MpvPreview::MpvPreview()
{
    mpv_observe_property(m_mpv, 0, "time-pos", MPV_FORMAT_DOUBLE);

    setProperty(MpvController::Properties::Mute, true);
    setProperty(MpvController::Properties::Pause, true);
    setProperty(MpvController::Properties::ReallyQuiet, true);

    setProperty(MpvController::Properties::HardwareDecoding, QStringLiteral("auto"));
    setProperty(MpvController::Properties::AccurateSeek, GeneralSettings::accuratePreviewThumbnail());
    setProperty(MpvController::Properties::AudioId, false);
    setProperty(MpvController::Properties::AudioFileAuto, false);
    setProperty(MpvController::Properties::SubtitleAuto, false);
    setProperty(MpvController::Properties::OsdLevel, 0);
    setProperty(MpvController::Properties::AudioPitchCorection, false);
    setProperty(MpvController::Properties::UseTextOsd, false);
    setProperty(MpvController::Properties::AudioDisplay, false);

    connect(this, &MpvPreview::fileChanged, this, &MpvPreview::loadFile);
    connect(this, &MpvPreview::ready, this, &MpvPreview::loadFile);
}

void MpvPreview::loadFile()
{
    if (m_mpv && m_mpv_gl && !m_file.isEmpty()) {
        command(QStringList() << QStringLiteral("loadfile") << m_file);
    }
}

double MpvPreview::position()
{
    return getCachedPropertyValue(QStringLiteral("time-pos")).toDouble();
}

void MpvPreview::setPosition(double value)
{
    if (qFuzzyCompare(m_position, value)) {
        return;
    }
    Q_EMIT setMpvProperty(QStringLiteral("time-pos"), value);
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
    setProperty(MpvController::Properties::AccurateSeek, _accuratePreview);
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
