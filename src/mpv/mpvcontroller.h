/*
 * SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef MPVCONTROLLER_H
#define MPVCONTROLLER_H

#include <QMap>
#include <QObject>

#include <mpv/client.h>
#include <mpv/render_gl.h>

/**
 * RAII wrapper that calls mpv_free_node_contents() on the pointer.
 */
struct node_autofree {
    mpv_node *ptr;
    explicit node_autofree(mpv_node *a_ptr)
        : ptr(a_ptr)
    {
    }
    ~node_autofree()
    {
        mpv_free_node_contents(ptr);
    }
};

/**
 * This is used to return error codes wrapped in QVariant for functions which
 * return QVariant.
 *
 * You can use get_error() or is_error() to extract the error status from a
 * QVariant value.
 */
struct ErrorReturn {
    /**
     * enum mpv_error value (or a value outside of it if ABI was extended)
     */
    int error;

    ErrorReturn()
        : error(0)
    {
    }
    explicit ErrorReturn(int err)
        : error(err)
    {
    }
};
Q_DECLARE_METATYPE(ErrorReturn)

class MpvController : public QObject
{
    Q_OBJECT
public:
    MpvController(QObject *parent = nullptr);

    enum class Properties {
        Pause,
        Volume,
        VolumeMax,
        HardwareDecoding,
        Position,
        Duration,
        Remaining,
        Mute,
        Deinterlace,
        Chapter,
        ChapterList,
        TrackList,
        MediaTitle,
        AudioId,
        AudioClientName,
        AudioLanguage,
        AudioFileAuto,
        AudioPitchCorection,
        AudioDisplay,
        SubtitleId,
        SubtitleLanguage,
        SubtitleDelay,
        SubtitleVisibility,
        SubtitleScale,
        SubtitleFilePaths,
        SubtitleAuto,
        SubtitleFont,
        SubtitleFontSize,
        SubtitleColor,
        SubtitleShadowColor,
        SubtitleShadowOffset,
        SubtitleBorderColor,
        SubtitleBorderSize,
        SubtitleBold,
        SubtitleItalic,
        SubtitleUseMargins,
        SubtitleAssForceMargins,
        SecondarySubtitleId,
        ScreenshotFormat,
        ScreenshotTemplate,
        VideoId,
        VideoZoom,
        VideoPanX,
        VideoPanY,
        LoopFile,
        ABLoopA,
        ABLoopB,
        YtdlFormat,
        ScriptOpts,
        ReallyQuiet,
        AccurateSeek,
        OsdLevel,
        UseTextOsd,
        Speed,
        Contrast,
        Brightness,
        Gamma,
        Saturation,
        Terminal,
    };
    Q_ENUM(Properties)

    enum class AsyncIds {
        FinishedLoading,
        SavePosition,
    };
    Q_ENUM(AsyncIds)

    /**
     * Set the given property as mpv_node converted from the QVariant argument.
     *
     * @return mpv error code (<0 on error, >= 0 on success)
     */
    Q_INVOKABLE int setProperty(const QString &name, const QVariant &value);
    Q_INVOKABLE int setProperty(Properties property, const QVariant &value);
    Q_INVOKABLE int setPropertyAsync(Properties property, const QVariant &value, AsyncIds id);

    /**
     * Return the given property as mpv_node converted to QVariant,
     * or QVariant() on error.
     *
     * @param `name` the property name
     * @return the property value, or an ErrorReturn with the error code
     */
    Q_INVOKABLE QVariant getProperty(const QString &name);
    Q_INVOKABLE int getPropertyAsync(Properties property, AsyncIds id);

    /**
     * mpv_command_node() equivalent.
     *
     * @param `args` command arguments, with args[0] being the command name as string
     * @return the property value, or an ErrorReturn with the error code
     */
    Q_INVOKABLE QVariant command(const QVariant &params);

    /**
     * Return an error string from an ErrorReturn.
     */
    Q_INVOKABLE QString getError(int error);

    static void mpvEvents(void *ctx);
    void eventHandler();
    mpv_handle *mpv() const;

    QMap<Properties, QString> properties() const;

Q_SIGNALS:
    void propertyChanged(const QString &property, const QVariant &value);
    void getPropertyReply(const QVariant &value, AsyncIds id);
    void setPropertyReply(AsyncIds id);
    void fileStarted();
    void fileLoaded();
    void endFile(QString reason);
    void videoReconfig();

private:
    mpv_node_list *create_list(mpv_node *dst, bool is_map, int num);
    void setNode(mpv_node *dst, const QVariant &src);
    bool test_type(const QVariant &v, QMetaType::Type t);
    void free_node(mpv_node *dst);
    QVariant node_to_variant(const mpv_node *node);

    mpv_handle *m_mpv{nullptr};
    // clang-format off
    QMap<Properties, QString> m_properties = {
        {Properties::Pause,                   QStringLiteral("pause")},
        {Properties::Volume,                  QStringLiteral("volume")},
        {Properties::VolumeMax,               QStringLiteral("volume-max")},
        {Properties::HardwareDecoding,        QStringLiteral("hwdec")},
        {Properties::Position,                QStringLiteral("time-pos")},
        {Properties::Duration,                QStringLiteral("duration")},
        {Properties::Remaining,               QStringLiteral("time-remaining")},
        {Properties::Mute,                    QStringLiteral("mute")},
        {Properties::Deinterlace,             QStringLiteral("deinterlace")},
        {Properties::Chapter,                 QStringLiteral("chapter")},
        {Properties::ChapterList,             QStringLiteral("chapter-list")},
        {Properties::TrackList,               QStringLiteral("track-list")},
        {Properties::MediaTitle,              QStringLiteral("media-title")},
        {Properties::AudioId,                 QStringLiteral("aid")},
        {Properties::AudioClientName,         QStringLiteral("audio-client-name")},
        {Properties::AudioLanguage,           QStringLiteral("alang")},
        {Properties::AudioFileAuto,           QStringLiteral("audio-file-auto")},
        {Properties::AudioPitchCorection,     QStringLiteral("audio-pitch-correction")},
        {Properties::AudioDisplay,            QStringLiteral("audio-display")},
        {Properties::SubtitleId,              QStringLiteral("sid")},
        {Properties::SubtitleLanguage,        QStringLiteral("slang")},
        {Properties::SubtitleDelay,           QStringLiteral("sub-delay")},
        {Properties::SubtitleVisibility,      QStringLiteral("sub-visibility")},
        {Properties::SubtitleScale,           QStringLiteral("sub-scale")},
        {Properties::SubtitleFilePaths,       QStringLiteral("sub-file-paths")},
        {Properties::SubtitleAuto,            QStringLiteral("sub-auto")},
        {Properties::SubtitleFont,            QStringLiteral("sub-font")},
        {Properties::SubtitleFontSize,        QStringLiteral("sub-font-size")},
        {Properties::SubtitleColor,           QStringLiteral("sub-color")},
        {Properties::SubtitleShadowColor,     QStringLiteral("sub-shadow-color")},
        {Properties::SubtitleShadowOffset,    QStringLiteral("sub-shadow-offset")},
        {Properties::SubtitleBorderColor,     QStringLiteral("sub-border-color")},
        {Properties::SubtitleBorderSize,      QStringLiteral("sub-border-size")},
        {Properties::SubtitleBold,            QStringLiteral("sub-bold")},
        {Properties::SubtitleItalic,          QStringLiteral("sub-italic")},
        {Properties::SubtitleUseMargins,      QStringLiteral("sub-use-margins")},
        {Properties::SubtitleAssForceMargins, QStringLiteral("sub-ass-force-margins")},
        {Properties::SecondarySubtitleId,     QStringLiteral("secondary-sid")},
        {Properties::ScreenshotFormat,        QStringLiteral("screenshot-template")},
        {Properties::ScreenshotTemplate,      QStringLiteral("screenshot-format")},
        {Properties::VideoId,                 QStringLiteral("vid")},
        {Properties::VideoZoom,               QStringLiteral("video-zoom")},
        {Properties::VideoPanX,               QStringLiteral("video-pan-x")},
        {Properties::VideoPanY,               QStringLiteral("video-pan-y")},
        {Properties::LoopFile,                QStringLiteral("loop-file")},
        {Properties::ABLoopA,                 QStringLiteral("ab-loop-a")},
        {Properties::ABLoopB,                 QStringLiteral("ab-loop-b")},
        {Properties::YtdlFormat,              QStringLiteral("ytdl-format")},
        {Properties::ScriptOpts,              QStringLiteral("script-opts")},
        {Properties::ReallyQuiet,             QStringLiteral("really-quiet")},
        {Properties::AccurateSeek,            QStringLiteral("hr-seek")},
        {Properties::OsdLevel,                QStringLiteral("osd-level")},
        {Properties::UseTextOsd,              QStringLiteral("use-text-osd")},
        {Properties::Speed,                   QStringLiteral("speed")},
        {Properties::Contrast,                QStringLiteral("contrast")},
        {Properties::Brightness,              QStringLiteral("brightness")},
        {Properties::Gamma,                   QStringLiteral("gamma")},
        {Properties::Saturation,              QStringLiteral("saturation")},
        {Properties::Terminal,                QStringLiteral("terminal")},
    };
    // clang-format on
};

#endif // MPVCONTROLLER_H
