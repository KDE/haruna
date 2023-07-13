#include "mpvpreview.h"

#include "generalsettings.h"

MpvPreview::MpvPreview()
{
    mpv_observe_property(m_mpv, 0, "time-pos", MPV_FORMAT_DOUBLE);

    setProperty(QStringLiteral("mute"), true);
    setProperty(QStringLiteral("pause"), true);
    setProperty(QStringLiteral("really-quiet"), true);

    setProperty(QStringLiteral("hwdec"), QStringLiteral("auto"));
    setProperty(QStringLiteral("hr-seek"), GeneralSettings::accuratePreviewThumbnail());
    setProperty(QStringLiteral("aid"), false);
    setProperty(QStringLiteral("audio-file-auto"), false);
    setProperty(QStringLiteral("sub-auto"), false);
    setProperty(QStringLiteral("osd-level"), 0);
    setProperty(QStringLiteral("audio-pitch-correction"), false);
    setProperty(QStringLiteral("use-text-osd"), false);
    setProperty(QStringLiteral("audio-display"), false);

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
    setProperty(QStringLiteral("hr-seek"), _accuratePreview);
    Q_EMIT accuratePreviewChanged();
}
