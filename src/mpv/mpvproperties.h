/*
 * SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef MPVPROPERTIES_H
#define MPVPROPERTIES_H

#include <QObject>
#include <qqmlintegration.h>

using namespace Qt::StringLiterals;

class MpvProperties : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    explicit MpvProperties(QObject *parent = nullptr)
        : QObject(parent)
    {}
    static MpvProperties *self()
    {
        static MpvProperties p;
        return &p;
    }

    Q_PROPERTY(QString VO MEMBER Pause CONSTANT)
    const QString VO{u"vo"_s};

    Q_PROPERTY(QString Pause MEMBER Pause CONSTANT)
    const QString Pause{u"pause"_s};

    Q_PROPERTY(QString Volume MEMBER Volume CONSTANT)
    const QString Volume{u"volume"_s};

    Q_PROPERTY(QString VolumeMax MEMBER VolumeMax CONSTANT)
    const QString VolumeMax{u"volume-max"_s};

    Q_PROPERTY(QString HardwareDecoding MEMBER HardwareDecoding CONSTANT)
    const QString HardwareDecoding{u"hwdec"_s};

    Q_PROPERTY(QString Position MEMBER Position CONSTANT)
    const QString Position{u"time-pos"_s};

    Q_PROPERTY(QString Duration MEMBER Duration CONSTANT)
    const QString Duration{u"duration"_s};

    Q_PROPERTY(QString Remaining MEMBER Remaining CONSTANT)
    const QString Remaining{u"time-remaining"_s};

    Q_PROPERTY(QString Mute MEMBER Mute CONSTANT)
    const QString Mute{u"mute"_s};

    Q_PROPERTY(QString Deinterlace MEMBER Deinterlace CONSTANT)
    const QString Deinterlace{u"deinterlace"_s};

    Q_PROPERTY(QString Chapter MEMBER Chapter CONSTANT)
    const QString Chapter{u"chapter"_s};

    Q_PROPERTY(QString ChapterList MEMBER ChapterList CONSTANT)
    const QString ChapterList{u"chapter-list"_s};

    Q_PROPERTY(QString TrackList MEMBER TrackList CONSTANT)
    const QString TrackList{u"track-list"_s};

    Q_PROPERTY(QString MediaTitle MEMBER MediaTitle CONSTANT)
    const QString MediaTitle{u"media-title"_s};

    Q_PROPERTY(QString AudioId MEMBER AudioId CONSTANT)
    const QString AudioId{u"aid"_s};

    Q_PROPERTY(QString AudioClientName MEMBER AudioClientName CONSTANT)
    const QString AudioClientName{u"audio-client-name"_s};

    Q_PROPERTY(QString AudioLanguage MEMBER AudioLanguage CONSTANT)
    const QString AudioLanguage{u"alang"_s};

    Q_PROPERTY(QString AudioFileAuto MEMBER AudioFileAuto CONSTANT)
    const QString AudioFileAuto{u"audio-file-auto"_s};

    Q_PROPERTY(QString AudioPitchCorection MEMBER AudioPitchCorection CONSTANT)
    const QString AudioPitchCorection{u"audio-pitch-correction"_s};

    Q_PROPERTY(QString AudioDisplay MEMBER AudioDisplay CONSTANT)
    const QString AudioDisplay{u"audio-display"_s};

    Q_PROPERTY(QString SubtitleId MEMBER SubtitleId CONSTANT)
    const QString SubtitleId{u"sid"_s};

    Q_PROPERTY(QString SecondarySubtitleId MEMBER SecondarySubtitleId CONSTANT)
    const QString SecondarySubtitleId{u"secondary-sid"_s};

    Q_PROPERTY(QString SubtitleLanguage MEMBER SubtitleLanguage CONSTANT)
    const QString SubtitleLanguage{u"slang"_s};

    Q_PROPERTY(QString SubtitleDelay MEMBER SubtitleDelay CONSTANT)
    const QString SubtitleDelay{u"sub-delay"_s};

    Q_PROPERTY(QString SubtitleVisibility MEMBER SubtitleVisibility CONSTANT)
    const QString SubtitleVisibility{u"sub-visibility"_s};

    Q_PROPERTY(QString SubtitleScale MEMBER SubtitleScale CONSTANT)
    const QString SubtitleScale{u"sub-scale"_s};

    Q_PROPERTY(QString SubtitleFilePaths MEMBER SubtitleFilePaths CONSTANT)
    const QString SubtitleFilePaths{u"sub-file-paths"_s};

    Q_PROPERTY(QString SubtitleAuto MEMBER SubtitleAuto CONSTANT)
    const QString SubtitleAuto{u"sub-auto"_s};

    Q_PROPERTY(QString SubtitleFont MEMBER SubtitleFont CONSTANT)
    const QString SubtitleFont{u"sub-font"_s};

    Q_PROPERTY(QString SubtitleFontSize MEMBER SubtitleFontSize CONSTANT)
    const QString SubtitleFontSize{u"sub-font-size"_s};

    Q_PROPERTY(QString SubtitleColor MEMBER SubtitleColor CONSTANT)
    const QString SubtitleColor{u"sub-color"_s};

    Q_PROPERTY(QString SubtitleShadowColor MEMBER SubtitleShadowColor CONSTANT)
    const QString SubtitleShadowColor{u"sub-shadow-color"_s};

    Q_PROPERTY(QString SubtitleShadowOffset MEMBER SubtitleShadowOffset CONSTANT)
    const QString SubtitleShadowOffset{u"sub-shadow-offset"_s};

    Q_PROPERTY(QString SubtitleBorderColor MEMBER SubtitleBorderColor CONSTANT)
    const QString SubtitleBorderColor{u"sub-border-color"_s};

    Q_PROPERTY(QString SubtitleBorderSize MEMBER SubtitleBorderSize CONSTANT)
    const QString SubtitleBorderSize{u"sub-border-size"_s};

    Q_PROPERTY(QString SubtitleBold MEMBER SubtitleBold CONSTANT)
    const QString SubtitleBold{u"sub-bold"_s};

    Q_PROPERTY(QString SubtitleItalic MEMBER SubtitleItalic CONSTANT)
    const QString SubtitleItalic{u"sub-italic"_s};

    Q_PROPERTY(QString SubtitleUseMargins MEMBER SubtitleUseMargins CONSTANT)
    const QString SubtitleUseMargins{u"sub-use-margins"_s};

    Q_PROPERTY(QString SubtitleAssForceMargins MEMBER SubtitleAssForceMargins CONSTANT)
    const QString SubtitleAssForceMargins{u"sub-ass-force-margins"_s};

    Q_PROPERTY(QString ScreenshotFormat MEMBER ScreenshotFormat CONSTANT)
    const QString ScreenshotFormat{u"screenshot-format"_s};

    Q_PROPERTY(QString ScreenshotTemplate MEMBER ScreenshotTemplate CONSTANT)
    const QString ScreenshotTemplate{u"screenshot-template"_s};

    Q_PROPERTY(QString VideoId MEMBER VideoId CONSTANT)
    const QString VideoId{u"vid"_s};

    Q_PROPERTY(QString VideoZoom MEMBER VideoZoom CONSTANT)
    const QString VideoZoom{u"video-zoom"_s};

    Q_PROPERTY(QString VideoPanX MEMBER VideoPanX CONSTANT)
    const QString VideoPanX{u"video-pan-x"_s};

    Q_PROPERTY(QString VideoPanY MEMBER VideoPanY CONSTANT)
    const QString VideoPanY{u"video-pan-y"_s};

    Q_PROPERTY(QString Width MEMBER Width CONSTANT)
    const QString Width{u"width"_s};

    Q_PROPERTY(QString Height MEMBER Height CONSTANT)
    const QString Height{u"height"_s};

    Q_PROPERTY(QString LoopFile MEMBER LoopFile CONSTANT)
    const QString LoopFile{u"loop-file"_s};

    Q_PROPERTY(QString ABLoopA MEMBER ABLoopA CONSTANT)
    const QString ABLoopA{u"ab-loop-a"_s};

    Q_PROPERTY(QString ABLoopB MEMBER ABLoopB CONSTANT)
    const QString ABLoopB{u"ab-loop-b"_s};

    Q_PROPERTY(QString YtdlFormat MEMBER YtdlFormat CONSTANT)
    const QString YtdlFormat{u"ytdl-format"_s};

    Q_PROPERTY(QString ScriptOpts MEMBER ScriptOpts CONSTANT)
    const QString ScriptOpts{u"script-opts"_s};

    Q_PROPERTY(QString ReallyQuiet MEMBER ReallyQuiet CONSTANT)
    const QString ReallyQuiet{u"really-quiet"_s};

    Q_PROPERTY(QString AccurateSeek MEMBER AccurateSeek CONSTANT)
    const QString AccurateSeek{u"hr-seek"_s};

    Q_PROPERTY(QString OsdLevel MEMBER OsdLevel CONSTANT)
    const QString OsdLevel{u"level"_s};

    Q_PROPERTY(QString UseTextOsd MEMBER UseTextOsd CONSTANT)
    const QString UseTextOsd{u"use-text-osd"_s};

    Q_PROPERTY(QString Speed MEMBER Speed CONSTANT)
    const QString Speed{u"speed"_s};

    Q_PROPERTY(QString Contrast MEMBER Contrast CONSTANT)
    const QString Contrast{u"contrast"_s};

    Q_PROPERTY(QString Brightness MEMBER Brightness CONSTANT)
    const QString Brightness{u"brightness"_s};

    Q_PROPERTY(QString Gamma MEMBER Gamma CONSTANT)
    const QString Gamma{u"gamma"_s};

    Q_PROPERTY(QString Saturation MEMBER Saturation CONSTANT)
    const QString Saturation{u"saturation"_s};

    Q_PROPERTY(QString Terminal MEMBER Terminal CONSTANT)
    const QString Terminal{u"terminal"_s};

    Q_PROPERTY(QString AspectRatio MEMBER AspectRatio CONSTANT)
    const QString AspectRatio{u"video-params/aspect"_s};

    Q_PROPERTY(QString TracksCount MEMBER TracksCount CONSTANT)
    const QString TracksCount{u"track-list/count"_s};

    Q_PROPERTY(QString KeepOpen MEMBER KeepOpen CONSTANT)
    const QString KeepOpen{u"keep-open"_s};

    Q_PROPERTY(QString EofReached MEMBER EofReached CONSTANT)
    const QString EofReached{u"eof-reached"_s};

private:
    Q_DISABLE_COPY_MOVE(MpvProperties)
};

#endif // MPVPROPERTIES_H
