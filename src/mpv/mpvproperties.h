/*
 * SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef MPVPROPERTIES_H
#define MPVPROPERTIES_H

#include <QObject>

class MpvProperties : public QObject
{
    Q_OBJECT

public:
    static MpvProperties *self()
    {
        static MpvProperties p;
        return &p;
    }

    Q_PROPERTY(QString VO MEMBER Pause CONSTANT)
    const QString VO{QStringLiteral("vo")};

    Q_PROPERTY(QString Pause MEMBER Pause CONSTANT)
    const QString Pause{QStringLiteral("pause")};

    Q_PROPERTY(QString Volume MEMBER Volume CONSTANT)
    const QString Volume{QStringLiteral("volume")};

    Q_PROPERTY(QString VolumeMax MEMBER VolumeMax CONSTANT)
    const QString VolumeMax{QStringLiteral("volume-max")};

    Q_PROPERTY(QString HardwareDecoding MEMBER HardwareDecoding CONSTANT)
    const QString HardwareDecoding{QStringLiteral("hwdec")};

    Q_PROPERTY(QString Position MEMBER Position CONSTANT)
    const QString Position{QStringLiteral("time-pos")};

    Q_PROPERTY(QString Duration MEMBER Duration CONSTANT)
    const QString Duration{QStringLiteral("duration")};

    Q_PROPERTY(QString Remaining MEMBER Remaining CONSTANT)
    const QString Remaining{QStringLiteral("time-remaining")};

    Q_PROPERTY(QString Mute MEMBER Mute CONSTANT)
    const QString Mute{QStringLiteral("mute")};

    Q_PROPERTY(QString Deinterlace MEMBER Deinterlace CONSTANT)
    const QString Deinterlace{QStringLiteral("deinterlace")};

    Q_PROPERTY(QString Chapter MEMBER Chapter CONSTANT)
    const QString Chapter{QStringLiteral("chapter")};

    Q_PROPERTY(QString ChapterList MEMBER ChapterList CONSTANT)
    const QString ChapterList{QStringLiteral("chapter-list")};

    Q_PROPERTY(QString TrackList MEMBER TrackList CONSTANT)
    const QString TrackList{QStringLiteral("track-list")};

    Q_PROPERTY(QString MediaTitle MEMBER MediaTitle CONSTANT)
    const QString MediaTitle{QStringLiteral("media-title")};

    Q_PROPERTY(QString AudioId MEMBER AudioId CONSTANT)
    const QString AudioId{QStringLiteral("aid")};

    Q_PROPERTY(QString AudioClientName MEMBER AudioClientName CONSTANT)
    const QString AudioClientName{QStringLiteral("audio-client-name")};

    Q_PROPERTY(QString AudioLanguage MEMBER AudioLanguage CONSTANT)
    const QString AudioLanguage{QStringLiteral("alang")};

    Q_PROPERTY(QString AudioFileAuto MEMBER AudioFileAuto CONSTANT)
    const QString AudioFileAuto{QStringLiteral("audio-file-auto")};

    Q_PROPERTY(QString AudioPitchCorection MEMBER AudioPitchCorection CONSTANT)
    const QString AudioPitchCorection{QStringLiteral("audio-pitch-correction")};

    Q_PROPERTY(QString AudioDisplay MEMBER AudioDisplay CONSTANT)
    const QString AudioDisplay{QStringLiteral("audio-display")};

    Q_PROPERTY(QString SubtitleId MEMBER SubtitleId CONSTANT)
    const QString SubtitleId{QStringLiteral("sid")};

    Q_PROPERTY(QString SecondarySubtitleId MEMBER SecondarySubtitleId CONSTANT)
    const QString SecondarySubtitleId{QStringLiteral("secondary-sid")};

    Q_PROPERTY(QString SubtitleLanguage MEMBER SubtitleLanguage CONSTANT)
    const QString SubtitleLanguage{QStringLiteral("slang")};

    Q_PROPERTY(QString SubtitleDelay MEMBER SubtitleDelay CONSTANT)
    const QString SubtitleDelay{QStringLiteral("sub-delay")};

    Q_PROPERTY(QString SubtitleVisibility MEMBER SubtitleVisibility CONSTANT)
    const QString SubtitleVisibility{QStringLiteral("sub-visibility")};

    Q_PROPERTY(QString SubtitleScale MEMBER SubtitleScale CONSTANT)
    const QString SubtitleScale{QStringLiteral("sub-scale")};

    Q_PROPERTY(QString SubtitleFilePaths MEMBER SubtitleFilePaths CONSTANT)
    const QString SubtitleFilePaths{QStringLiteral("sub-file-paths")};

    Q_PROPERTY(QString SubtitleAuto MEMBER SubtitleAuto CONSTANT)
    const QString SubtitleAuto{QStringLiteral("sub-auto")};

    Q_PROPERTY(QString SubtitleFont MEMBER SubtitleFont CONSTANT)
    const QString SubtitleFont{QStringLiteral("sub-font")};

    Q_PROPERTY(QString SubtitleFontSize MEMBER SubtitleFontSize CONSTANT)
    const QString SubtitleFontSize{QStringLiteral("sub-font-size")};

    Q_PROPERTY(QString SubtitleColor MEMBER SubtitleColor CONSTANT)
    const QString SubtitleColor{QStringLiteral("sub-color")};

    Q_PROPERTY(QString SubtitleShadowColor MEMBER SubtitleShadowColor CONSTANT)
    const QString SubtitleShadowColor{QStringLiteral("sub-shadow-color")};

    Q_PROPERTY(QString SubtitleShadowOffset MEMBER SubtitleShadowOffset CONSTANT)
    const QString SubtitleShadowOffset{QStringLiteral("sub-shadow-offset")};

    Q_PROPERTY(QString SubtitleBorderColor MEMBER SubtitleBorderColor CONSTANT)
    const QString SubtitleBorderColor{QStringLiteral("sub-border-color")};

    Q_PROPERTY(QString SubtitleBorderSize MEMBER SubtitleBorderSize CONSTANT)
    const QString SubtitleBorderSize{QStringLiteral("sub-border-size")};

    Q_PROPERTY(QString SubtitleBold MEMBER SubtitleBold CONSTANT)
    const QString SubtitleBold{QStringLiteral("sub-bold")};

    Q_PROPERTY(QString SubtitleItalic MEMBER SubtitleItalic CONSTANT)
    const QString SubtitleItalic{QStringLiteral("sub-italic")};

    Q_PROPERTY(QString SubtitleUseMargins MEMBER SubtitleUseMargins CONSTANT)
    const QString SubtitleUseMargins{QStringLiteral("sub-use-margins")};

    Q_PROPERTY(QString SubtitleAssForceMargins MEMBER SubtitleAssForceMargins CONSTANT)
    const QString SubtitleAssForceMargins{QStringLiteral("sub-ass-force-margins")};

    Q_PROPERTY(QString ScreenshotFormat MEMBER ScreenshotFormat CONSTANT)
    const QString ScreenshotFormat{QStringLiteral("screenshot-format")};

    Q_PROPERTY(QString ScreenshotTemplate MEMBER ScreenshotTemplate CONSTANT)
    const QString ScreenshotTemplate{QStringLiteral("screenshot-template")};

    Q_PROPERTY(QString VideoId MEMBER VideoId CONSTANT)
    const QString VideoId{QStringLiteral("vid")};

    Q_PROPERTY(QString VideoZoom MEMBER VideoZoom CONSTANT)
    const QString VideoZoom{QStringLiteral("video-zoom")};

    Q_PROPERTY(QString VideoPanX MEMBER VideoPanX CONSTANT)
    const QString VideoPanX{QStringLiteral("video-pan-x")};

    Q_PROPERTY(QString VideoPanY MEMBER VideoPanY CONSTANT)
    const QString VideoPanY{QStringLiteral("video-pan-y")};

    Q_PROPERTY(QString Width MEMBER Width CONSTANT)
    const QString Width{QStringLiteral("width")};

    Q_PROPERTY(QString Height MEMBER Height CONSTANT)
    const QString Height{QStringLiteral("height")};

    Q_PROPERTY(QString LoopFile MEMBER LoopFile CONSTANT)
    const QString LoopFile{QStringLiteral("loop-file")};

    Q_PROPERTY(QString ABLoopA MEMBER ABLoopA CONSTANT)
    const QString ABLoopA{QStringLiteral("ab-loop-a")};

    Q_PROPERTY(QString ABLoopB MEMBER ABLoopB CONSTANT)
    const QString ABLoopB{QStringLiteral("ab-loop-b")};

    Q_PROPERTY(QString YtdlFormat MEMBER YtdlFormat CONSTANT)
    const QString YtdlFormat{QStringLiteral("ytdl-format")};

    Q_PROPERTY(QString ScriptOpts MEMBER ScriptOpts CONSTANT)
    const QString ScriptOpts{QStringLiteral("script-opts")};

    Q_PROPERTY(QString ReallyQuiet MEMBER ReallyQuiet CONSTANT)
    const QString ReallyQuiet{QStringLiteral("really-quiet")};

    Q_PROPERTY(QString AccurateSeek MEMBER AccurateSeek CONSTANT)
    const QString AccurateSeek{QStringLiteral("hr-seek")};

    Q_PROPERTY(QString OsdLevel MEMBER OsdLevel CONSTANT)
    const QString OsdLevel{QStringLiteral("level")};

    Q_PROPERTY(QString UseTextOsd MEMBER UseTextOsd CONSTANT)
    const QString UseTextOsd{QStringLiteral("use-text-osd")};

    Q_PROPERTY(QString Speed MEMBER Speed CONSTANT)
    const QString Speed{QStringLiteral("speed")};

    Q_PROPERTY(QString Contrast MEMBER Contrast CONSTANT)
    const QString Contrast{QStringLiteral("contrast")};

    Q_PROPERTY(QString Brightness MEMBER Brightness CONSTANT)
    const QString Brightness{QStringLiteral("brightness")};

    Q_PROPERTY(QString Gamma MEMBER Gamma CONSTANT)
    const QString Gamma{QStringLiteral("gamma")};

    Q_PROPERTY(QString Saturation MEMBER Saturation CONSTANT)
    const QString Saturation{QStringLiteral("saturation")};

    Q_PROPERTY(QString Terminal MEMBER Terminal CONSTANT)
    const QString Terminal{QStringLiteral("terminal")};

    Q_PROPERTY(QString AspectRatio MEMBER AspectRatio CONSTANT)
    const QString AspectRatio{QStringLiteral("video-params/aspect")};

    Q_PROPERTY(QString TracksCount MEMBER TracksCount CONSTANT)
    const QString TracksCount{QStringLiteral("track-list/count")};

private:
    explicit MpvProperties(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

    MpvProperties(const MpvProperties &) = delete;
    MpvProperties &operator=(const MpvProperties &) = delete;
    MpvProperties(MpvProperties &&) = delete;
    MpvProperties &operator=(MpvProperties &&) = delete;
};

#endif // MPVPROPERTIES_H
