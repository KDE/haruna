/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 * SPDX-FileCopyrightText: 2025 Muhammet Sadık Uğursoy <sadikugursoy@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "mpvitem.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QDir>
#include <QTimer>

#include <KFileMetaData/ExtractorCollection>
#include <KFileMetaData/SimpleExtractionResult>
#include <KLocalizedString>
#include <KShell>
#include <MpvController>

#include "application.h"
#include "audiosettings.h"
#include "chaptersmodel.h"
#include "config-haruna.h"
#include "database.h"
#include "generalsettings.h"
#include "lockmanager.h"
#include "miscutils.h"
#include "mpvproperties.h"
#include "pathutils.h"
#include "playbacksettings.h"
#include "playlistsettings.h"
#include "playlistsmanager.h"
#include "subtitlessettings.h"
#include "tracksmodel.h"
#include "videosettings.h"
#include "worker.h"
#include "youtube.h"

#if HAVE_DBUS
#include <QDBusConnection>
#include <QDBusMessage>

#include "mediaplayer2.h"
#include "mediaplayer2player.h"
#endif

using namespace Qt::StringLiterals;

MpvItem::MpvItem(QQuickItem *parent)
    : MpvAbstractItem(parent)
    , m_audioTracksModel{std::make_unique<TracksModel>()}
    , m_subtitleTracksModel{std::make_unique<TracksModel>()}
    , m_chaptersModel{std::make_unique<ChaptersModel>()}
    , m_saveTimePositionTimer{std::make_unique<QTimer>()}
{
    observeProperty(MpvProperties::self()->MediaTitle, MPV_FORMAT_STRING);
    observeProperty(MpvProperties::self()->Position, MPV_FORMAT_DOUBLE);
    observeProperty(MpvProperties::self()->Remaining, MPV_FORMAT_DOUBLE);
    observeProperty(MpvProperties::self()->Duration, MPV_FORMAT_DOUBLE);
    observeProperty(MpvProperties::self()->Pause, MPV_FORMAT_FLAG);
    observeProperty(MpvProperties::self()->Volume, MPV_FORMAT_INT64);
    observeProperty(MpvProperties::self()->VolumeMax, MPV_FORMAT_INT64);
    observeProperty(MpvProperties::self()->Mute, MPV_FORMAT_FLAG);
    observeProperty(MpvProperties::self()->AudioId, MPV_FORMAT_INT64);
    observeProperty(MpvProperties::self()->SubtitleId, MPV_FORMAT_INT64);
    observeProperty(MpvProperties::self()->Width, MPV_FORMAT_NODE);
    observeProperty(MpvProperties::self()->Height, MPV_FORMAT_NODE);
    observeProperty(MpvProperties::self()->SecondarySubtitleId, MPV_FORMAT_INT64);
    observeProperty(MpvProperties::self()->Chapter, MPV_FORMAT_INT64);
    observeProperty(MpvProperties::self()->ChapterList, MPV_FORMAT_NODE);
    observeProperty(MpvProperties::self()->TracksCount, MPV_FORMAT_NODE);
    observeProperty(MpvProperties::self()->SubtitleDelay, MPV_FORMAT_DOUBLE);
    observeProperty(MpvProperties::self()->EofReached, MPV_FORMAT_FLAG);
    observeProperty(MpvProperties::self()->VoConfigured, MPV_FORMAT_FLAG);

    setupConnections();
    initProperties();

    KAboutComponent mpvComponent(u"mpv"_s,
                                 i18n("Command line video player"),
                                 getProperty(u"mpv-version"_s).toString().replace(u"mpv "_s, QString{}),
                                 u"https://mpv.io"_s,
                                 KAboutLicense::GPL);
    Application::instance()->aboutDataAddComponent(mpvComponent);

    KAboutComponent ffmpegComponent(u"ffmpeg"_s,
                                    i18n("Cross-platform solution to record, convert and stream audio and video"),
                                    getProperty(u"ffmpeg-version"_s).toString(),
                                    u"https://www.ffmpeg.org"_s,
                                    KAboutLicense::GPL);
    Application::instance()->aboutDataAddComponent(ffmpegComponent);

    m_saveTimePositionTimer->setInterval(PlaybackSettings::savePositionInterval() * 1000);
    m_saveTimePositionTimer->start();

    connect(m_saveTimePositionTimer.get(), &QTimer::timeout, this, [this]() {
        if (finishedLoading() && duration() > 0 && !pause()) {
            if (position() < duration() - 10) {
                saveTimePosition();
            } else {
                resetTimePosition();
            }
        }
    });

    connect(QApplication::instance(), &QApplication::aboutToQuit, this, [this]() {
        if (playbackState() == PlaybackState::Stopped) {
            return;
        }
        if (position() < duration() - 10) {
            saveTimePosition();
        } else {
            resetTimePosition();
        }
    });

    connect(this, &MpvAbstractItem::ready, this, &MpvItem::onReady);

    // run user commands
    KSharedConfig::Ptr m_customPropsConfig;
    QString ccConfig = PathUtils::instance()->configFilePath(PathUtils::ConfigFile::CustomCommands);
    m_customPropsConfig = KSharedConfig::openConfig(ccConfig, KConfig::SimpleConfig);
    QStringList groups = m_customPropsConfig->groupList();
    for (const QString &_group : std::as_const(groups)) {
        auto configGroup = m_customPropsConfig->group(_group);
        QString type = configGroup.readEntry("Type", QString());
        bool setOnStartup = configGroup.readEntry("SetOnStartup", true);
        if (type == u"startup"_s && setOnStartup) {
            userCommand(configGroup.readEntry("Command", QString()));
        }
    }
}

MpvItem::~MpvItem() = default;

void MpvItem::initProperties()
{
    //    setProperty(u"terminal"_s, InformationSettings::mpvLogging());
    //    setProperty(u"msg-level"_s, u"all=v"_s);

    setProperty(u"reset-on-next-file"_s, QStringList{MpvProperties::self()->ABLoopA, MpvProperties::self()->ABLoopB});

    setProperty(MpvProperties::self()->VO, u"libmpv"_s);
    setProperty(MpvProperties::self()->Pause, m_pause);

    setProperty(MpvProperties::self()->HardwareDecoding, PlaybackSettings::hWDecoding());
    setProperty(MpvProperties::self()->Volume, AudioSettings::volume());
    setProperty(MpvProperties::self()->VolumeMax, 100);

    // set ytdl_path to yt-dlp or fallback to youtube-dl
    YouTube yt;
    setProperty(MpvProperties::self()->ScriptOpts, u"ytdl_hook-ytdl_path=%1"_s.arg(yt.youtubeDlExecutable()));
    QCommandLineParser *cmdParser = Application::instance()->parser();
    QString ytdlFormat = PlaybackSettings::ytdlFormat();
    if (cmdParser->isSet(u"ytdl-format-selection"_s)) {
        ytdlFormat = cmdParser->value(u"ytdl-format-selection"_s);
    }
    setProperty(MpvProperties::self()->YtdlFormat, ytdlFormat);

    setProperty(MpvProperties::self()->SubtitleAuto, u"fuzzy"_s);
    setProperty(MpvProperties::self()->SubtitleUseMargins, SubtitlesSettings::allowOnBlackBorders());
    setProperty(MpvProperties::self()->SubtitleAssForceMargins, SubtitlesSettings::allowOnBlackBorders());
    setProperty(MpvProperties::self()->SubtitleFont, SubtitlesSettings::fontFamily());
    setProperty(MpvProperties::self()->SubtitleFontSize, SubtitlesSettings::fontSize());
    setProperty(MpvProperties::self()->SubtitleColor, SubtitlesSettings::fontColor());
    setProperty(MpvProperties::self()->SubtitleShadowColor, SubtitlesSettings::shadowColor());
    setProperty(MpvProperties::self()->SubtitleShadowOffset, SubtitlesSettings::shadowOffset());
    setProperty(MpvProperties::self()->SubtitleBorderColor, SubtitlesSettings::borderColor());
    setProperty(MpvProperties::self()->SubtitleBorderSize, SubtitlesSettings::borderSize());
    setProperty(MpvProperties::self()->SubtitleBold, SubtitlesSettings::isBold());
    setProperty(MpvProperties::self()->SubtitleItalic, SubtitlesSettings::isItalic());
    setProperty(MpvProperties::self()->SubtitleBorderStyle, SubtitlesSettings::borderStyle());
    setProperty(MpvProperties::self()->ReplayGain, AudioSettings::replayGain());
    setProperty(MpvProperties::self()->ReplayGainClip, !AudioSettings::replayGainPreventClip());
    setProperty(MpvProperties::self()->ReplayGainPreamp, AudioSettings::replayGainPreamp());
    setProperty(MpvProperties::self()->ReplayGainFallback, AudioSettings::replayGainFallback());

    auto setSubPaths = [this]() {
        QString subFoldersString;
        const auto subFolders{SubtitlesSettings::subtitlesFolders()};

        for (const auto &sf : subFolders) {
            if (sf.startsWith(u"*"_s)) {
                if (!SubtitlesSettings::recursiveSubtitlesSearch()) {
                    // only add relative folders starting with * if recursive search is disabled
                    // otherwise both recursive search and mpv will find and add the same sub file
                    QString _sf{sf};
                    _sf = _sf.removeFirst();
                    subFoldersString.append(u"%1:"_s.arg(_sf));
                }
            } else {
                subFoldersString.append(u"%1:"_s.arg(sf));
            }
        }
        subFoldersString.removeLast();

        setProperty(MpvProperties::self()->SubtitleFilePaths, subFoldersString);
    };
    setSubPaths();
    connect(SubtitlesSettings::self(), &SubtitlesSettings::RecursiveSubtitlesSearchChanged, this, setSubPaths);
    selectSubtitleTrack();

    setProperty(MpvProperties::self()->ScreenshotTemplate, VideoSettings::screenshotTemplate());
    setProperty(MpvProperties::self()->ScreenshotFormat, VideoSettings::screenshotFormat());

    setProperty(MpvProperties::self()->AudioClientName, u"haruna"_s);
    const QVariant preferredAudioTrack = AudioSettings::preferredTrack();
    setProperty(MpvProperties::self()->AudioId, preferredAudioTrack == 0 ? u"auto"_s : preferredAudioTrack);
    setProperty(MpvProperties::self()->AudioLanguage, AudioSettings::preferredLanguage().remove(u" "_s));
}

void MpvItem::setupConnections()
{
    // clang-format off
    connect(mpvController(), &MpvController::propertyChanged,
            this, &MpvItem::onPropertyChanged, Qt::QueuedConnection);

    connect(mpvController(), &MpvController::fileStarted,
            this, &MpvItem::fileStarted, Qt::QueuedConnection);

    connect(mpvController(), &MpvController::endFile,
            this, &MpvItem::onEndFile, Qt::QueuedConnection);

    connect(this, &MpvItem::eofReached,
            this, &MpvItem::onEndOfFileReached);

    connect(mpvController(), &MpvController::videoReconfig,
            this, &MpvItem::videoReconfig, Qt::QueuedConnection);

    connect(mpvController(), &MpvController::asyncReply,
            this, &MpvItem::onAsyncReply, Qt::QueuedConnection);

    connect(this, &MpvItem::currentUrlChanged, this, [this]() {
        setFinishedLoading(false);
    });

    connect(mpvController(), &MpvController::fileLoaded,
            this, &MpvItem::onFileLoaded, Qt::QueuedConnection);

    connect(this, &MpvItem::positionChanged, this, [this]() {
        auto pos = static_cast<int>(position());
        if (!m_secondsWatched.contains(pos)) {
            m_secondsWatched << pos;
            if (m_duration != 0) {
                setWatchPercentage(m_secondsWatched.count() * 100 / m_duration);
            }
        }
    });

    connect(Worker::instance(), &Worker::subtitlesFound, this, [this](QStringList subs) {
        for (const auto &sub : std::as_const(subs)) {
            commandBlocking(QStringList{u"sub-add"_s, sub, u"select"_s});
        }
        getPropertyAsync(MpvProperties::self()->TrackList, static_cast<int>(AsyncIds::TrackList));
    });

    connect(this, &MpvItem::chapterChanged,
            this, &MpvItem::onChapterChanged);

#if HAVE_DBUS
    // register mpris dbus service
    QString mspris2Name(u"org.mpris.MediaPlayer2.haruna"_s);
    QDBusConnection::sessionBus().registerService(mspris2Name);
    QDBusConnection::sessionBus().registerObject(u"/org/mpris/MediaPlayer2"_s, this, QDBusConnection::ExportAdaptors);
    // org.mpris.MediaPlayer2 mpris2 interface
    auto *mp2 = new MediaPlayer2(this);
    connect(mp2, &MediaPlayer2::raise, this, &MpvItem::raise);
    auto *mp2Player = new MediaPlayer2Player(this);
    connect(mp2Player, &MediaPlayer2Player::playpause, this, [this]() {
        setPause(!pause());
    });
    connect(mp2Player, &MediaPlayer2Player::play, this, [this]() {
        setPause(false);
    });
    connect(mp2Player, &MediaPlayer2Player::pause, this, [this]() {
        setPause(true);
    });
    connect(mp2Player, &MediaPlayer2Player::stop, this, [this]() {
        setPosition(0);
        setPause(true);
        stop();
    });
    connect(mp2Player, &MediaPlayer2Player::next, this, [this]() {
        Q_EMIT playNext();
    });
    connect(mp2Player, &MediaPlayer2Player::previous, this, [this]() {
        Q_EMIT playPrevious();
    });
    connect(mp2Player, &MediaPlayer2Player::seek, this, [this](int offset) {
        command(QStringList() << u"add"_s << u"time-pos"_s << QString::number(offset));
    });
    connect(mp2Player, &MediaPlayer2Player::openUri, this, [this](const QString &uri) {
        Q_EMIT openUri(uri);
    });
#endif

    connect(this, &MpvItem::pauseChanged, this, [this]() {
        static LockManager lockManager;
        if (pause()) {
            lockManager.setInhibitionOff();
        } else {
            if (!m_currentUrl.isEmpty()) {
                lockManager.setInhibitionOn();
            }
        }
    });

    connect(this, &MpvItem::savePositionToDB, Worker::instance(), &Worker::savePositionToDB, Qt::QueuedConnection);
    // clang-format on
}

void MpvItem::onReady()
{
    setIsReady(true);
}

void MpvItem::onFileLoaded()
{
    getPropertyAsync(MpvProperties::self()->ChapterList, static_cast<int>(AsyncIds::ChapterList));
    getPropertyAsync(MpvProperties::self()->VideoId, static_cast<int>(AsyncIds::VideoId));
    getPropertyAsync(MpvProperties::self()->TrackList, static_cast<int>(AsyncIds::TrackList));

    // clang-format off
    auto pause = PlaybackSettings::restoreFilePosition()
            ? !PlaybackSettings::playOnResume() && watchLaterPosition() > 1
            : false;
    // clang-format on
    setPause(pause);

    if (SubtitlesSettings::recursiveSubtitlesSearch()) {
        QMetaObject::invokeMethod(Worker::instance(), &Worker::findRecursiveSubtitles, Qt::QueuedConnection, m_currentUrl);
    }

    GeneralSettings::setLastPlayedFile(m_currentUrl.toString());
    GeneralSettings::self()->save();
    auto state = pause ? PlaybackState::Paused : PlaybackState::Playing;
    setPlaybackState(state);

    setFinishedLoading(true);
}

void MpvItem::onEndFile(const QString &reason)
{
    // this runs after the file has been unloaded from mpv
    if (reason == u"error"_s) {
        auto *playlistModel = playlistsManager()->activePlaylist();
        if (playlistModel->rowCount() == 0) {
            return;
        }

        uint currentItem = playlistModel->getPlayingItem();
        const auto index = playlistModel->index(currentItem, 0);
        const auto title = playlistModel->data(index, PlaylistModel::TitleRole);

        Q_EMIT MiscUtils::instance()->error(i18nc("@info:tooltip; %1 is a video title/filename", "Could not play: %1", title.toString()));
        return;
    }
}

void MpvItem::onEndOfFileReached()
{
    switch (endOfFileAction()) {
    case EndOfFileAction::PlayNext:
        playlistsManager()->playNext();
        Q_EMIT eofPlayNext();
        break;
    case EndOfFileAction::Replay:
        setPropertyBlocking(MpvProperties::self()->Position, 0);
        setPropertyBlocking(MpvProperties::self()->Pause, false);
        break;
    case EndOfFileAction::Stop:
        stop();
        break;
    default:
        stop();
        break;
    }
}

MpvItem::EndOfFileAction MpvItem::endOfFileAction() const
{
    auto *const playlistModel = playlistsManager()->activePlaylist();
    const auto behavior = PlaylistSettings::playbackBehavior();
    if (behavior == u"RepeatItem"_s) {
        return EndOfFileAction::Replay;
    }

    if (behavior == u"StopAfterItem"_s) {
        return EndOfFileAction::Stop;
    }

    if (behavior == u"StopAfterLast"_s) {
        if (playlistModel->isLastItem(playlistModel->getPlayingItem())) {
            return EndOfFileAction::Stop;
        }

        return EndOfFileAction::PlayNext;
    }

    if (behavior == u"RepeatPlaylist"_s) {
        if (playlistModel->rowCount() == 1) {
            return EndOfFileAction::Replay;
        }

        return EndOfFileAction::PlayNext;
    }

    return EndOfFileAction::Stop;
}

void MpvItem::onPropertyChanged(const QString &property, const QVariant &value)
{
    if (property == MpvProperties::self()->MediaTitle) {
        m_mediaTitle = value.toString();
        Q_EMIT mediaTitleChanged();

    } else if (property == MpvProperties::self()->Position) {
        m_position = value.toDouble();
        m_formattedPosition = MiscUtils::formatTime(m_position);
        updateTaskbarPlaybackProgress();
        Q_EMIT positionChanged();

    } else if (property == MpvProperties::self()->Remaining) {
        m_remaining = value.toDouble();
        m_formattedRemaining = MiscUtils::formatTime(m_remaining);
        Q_EMIT remainingChanged();

    } else if (property == MpvProperties::self()->Duration) {
        m_duration = value.toDouble();
        m_formattedDuration = MiscUtils::formatTime(m_duration);
        updateTaskbarPlaybackProgress();
        Q_EMIT durationChanged();

    } else if (property == MpvProperties::self()->Pause) {
        m_pause = value.toBool();

        auto state = m_pause ? PlaybackState::Paused : PlaybackState::Playing;
        if (playbackState() == PlaybackState::Stopped) {
            state = PlaybackState::Stopped;
        }
        setPlaybackState(state);

        if (state == PlaybackState::Stopped && !m_pause && m_currentUrl.isValid()) {
            loadFile(m_currentUrl.toString());
            playlistsManager()->activePlaylist()->setIsPlaying(true);
        }

        Q_EMIT pauseChanged();

    } else if (property == MpvProperties::self()->Volume) {
        m_volume = value.toInt();
        Q_EMIT volumeChanged();

        AudioSettings::setVolume(m_volume);
        AudioSettings::self()->save();

        Q_EMIT osdMessage(i18nc("@info:tooltip", "Volume: %1", m_volume));

    } else if (property == MpvProperties::self()->VolumeMax) {
        m_volumeMax = value.toInt();
        Q_EMIT volumeMaxChanged();

    } else if (property == MpvProperties::self()->Mute) {
        m_mute = value.toBool();
        Q_EMIT muteChanged();

    } else if (property == MpvProperties::self()->Chapter) {
        m_chapter = value.toInt();
        Q_EMIT chapterChanged();

    } else if (property == MpvProperties::self()->ChapterList) {
        Q_EMIT chapterListChanged();

    } else if (property == MpvProperties::self()->AudioId) {
        m_audioId = value.toInt();
        Q_EMIT audioIdChanged();

    } else if (property == MpvProperties::self()->SubtitleId) {
        m_subtitleId = value.toInt();
        Q_EMIT subtitleIdChanged();

    } else if (property == MpvProperties::self()->SecondarySubtitleId) {
        m_secondarySubtitleId = value.toInt();
        Q_EMIT secondarySubtitleIdChanged();

    } else if (property == MpvProperties::self()->SubtitleDelay) {
        auto delay = QString::number(value.toDouble(), 'f', 2).toDouble();
        auto delayString = QLocale().toString(delay, 'f', 2);
        if (delay > 0) {
            Q_EMIT osdMessage(i18nc("@info:tooltip", "Subtitle timing: +%1 seconds", delayString));
        } else {
            Q_EMIT osdMessage(i18nc("@info:tooltip", "Subtitle timing: %1 seconds", delayString));
        }

    } else if (property == MpvProperties::self()->Width) {
        m_videoWidth = value.toInt();
        Q_EMIT videoWidthChanged();

    } else if (property == MpvProperties::self()->Height) {
        m_videoHeight = value.toInt();
        Q_EMIT videoHeightChanged();

    } else if (property == MpvProperties::self()->EofReached) {
        if (value.toBool()) {
            Q_EMIT eofReached();
        }

    } else if (property == MpvProperties::self()->VoConfigured) {
        // needed to ensure the video output is cleared
        // since calling update() after the stop command is not enough
        bool voConfigured = value.toBool();
        if (!voConfigured) {
            update();
        }
    }
}

void MpvItem::loadFile(const QString &file)
{
    // must be set to always for the playback behavior to work as intended
    setProperty(MpvProperties::self()->KeepOpen, u"always"_s);

    auto url = QUrl::fromUserInput(file);
    if (m_currentUrl != url) {
        m_currentUrl = url;
        Q_EMIT currentUrlChanged();
    }

    QFileInfo fi{url.toString(QUrl::PreferLocalFile)};
    if (url.isLocalFile() && !fi.exists()) {
        m_currentUrl = {};
        Q_EMIT currentUrlChanged();
        Q_EMIT MiscUtils::instance()->error(i18nc("@info:tooltip; %1 is an absolute path", "File doesn't exist: %1", file));
        return;
    }
    // must be called after m_currentUrl changes
    setPlaybackState(PlaybackState::Loading);

    setWatchLaterPosition(loadTimePosition());
    if (PlaybackSettings::restoreFilePosition()) {
        // faster than seeking after loading the file
        setPropertyBlocking(u"start"_s, QString::number(m_watchLaterPosition));
    } else {
        // `start` persists through `stop` for the same file
        // if a file is reopened after being stopped the last `start` value will be used
        // which is not the wanted behavior when PlaybackSettings::restoreFilePosition if disabled
        setPropertyBlocking(u"start"_s, QString::number(0));
    }

    // store the mute property so it can be restored after loading file
    auto mute = m_mute;
    // mute to avoid popping sound while loading files
    setPropertyBlocking(MpvProperties::self()->Mute, true);
    command(QStringList() << u"loadfile"_s << m_currentUrl.toString());
    setPropertyBlocking(MpvProperties::self()->Mute, mute);

    const auto *playlistModel = playlistsManager()->activePlaylist()->playlistModel();
    const auto playingRow = playlistModel->playingItem();
    const auto playingIndex = playlistModel->index(playingRow, 0);
    const auto title = playlistModel->data(playingIndex, PlaylistModel::Roles::TitleRole).toString();
    const auto filename = playlistModel->data(playingIndex, PlaylistModel::Roles::NameRole).toString();
    const auto itemName = PlaylistSettings::showMediaTitle() ? title : filename;
    // clang-format off
    const auto msg = i18nc("%1 current item number, %2 total items count, %3 item name",
                           "[%1/%2] %3", playingRow + 1, playlistModel->rowCount(), itemName);
    // clang-format on
    Q_EMIT osdMessage(msg);
}

void MpvItem::loadTracks(const QList<QVariant> &tracks)
{
    m_subtitleTracksModel->clear();
    m_audioTracksModel->clear();

    Track noneTrack = {
        0,
        QString{},
        i18nc("@action The \"None\" subtitle track is used to clear/unset selected track", "None"),
        QString{},
    };
    m_subtitleTracksModel->addTrack(noneTrack);

    uint ar = 0;
    uint sr = 0;
    for (const auto &item : tracks) {
        const auto map = item.toMap();
        Track track = {
            map.value(u"id"_s).toInt(),
            map.value(u"lang"_s).toString(),
            map.value(u"title"_s).toString(),
            map.value(u"codec"_s).toString(),
        };
        const auto type = map.value(u"type"_s).toString();
        if (type == u"sub"_s) {
            if (track.trackid == subtitleId()) {
                m_subtitleTracksModel->setActiveRow(sr);
            }
            m_subtitleTracksModel->addTrack(track);
            sr++;
        }
        if (type == u"audio"_s) {
            if (track.trackid == audioId()) {
                m_audioTracksModel->setActiveRow(ar);
            }
            m_audioTracksModel->addTrack(track);
            ar++;
        }
    }

    Q_EMIT audioTracksModelChanged();
    Q_EMIT subtitleTracksModelChanged();
}

void MpvItem::onAsyncReply(const QVariant &data, mpv_event event)
{
    switch (static_cast<AsyncIds>(event.reply_userdata)) {
    case AsyncIds::None: {
        break;
    }
    case AsyncIds::SavePosition: {
        auto hash = MiscUtils::md5(currentUrl().toString());
        Q_EMIT savePositionToDB(hash, currentUrl().toString(), data.toDouble());
        break;
    }
    case AsyncIds::Screenshot: {
        if (event.error < 0) {
            Q_EMIT osdMessage(i18nc("@info:tooltip osd", "Screenshot failed"));
            break;
        }
        auto filename = data.toMap().value(u"filename"_s).toString();
        if (filename.isEmpty()) {
            Q_EMIT osdMessage(i18nc("@info:tooltip osd", "Screenshot taken"));
        } else {
            Q_EMIT osdMessage(i18nc("@info:tooltip osd; %1 is a filename with path", "Screenshot: %1", filename));
        }
        break;
    }
    case AsyncIds::TrackList: {
        loadTracks(data.toList());
        break;
    }
    case AsyncIds::ChapterList: {
        QList<Chapter> chaptersList;
        for (const auto &chapter : data.toList()) {
            const auto map = chapter.toMap();
            Chapter c;
            c.title = map.value(u"title"_s).toString();
            c.startTime = map.value(u"time"_s).toDouble();
            chaptersList.append(c);
        }
        m_chaptersModel->setChapters(chaptersList);
        break;
    }
    case AsyncIds::VideoId: {
        if (!data.toBool()) {
            // there's no video track
            // either because the file is an audio file or the video track can't be decoded
            auto mimeType = MiscUtils::mimeType(currentUrl());
            if (mimeType.startsWith(u"video/"_s)) {
                auto errMsg = i18nc("Error message when video can't be decoded/played",
                                    "No video track detected, most likely the video track can't be decoded/played due to missing codecs");
                Q_EMIT MiscUtils::instance()->error(errMsg);
                break;
            }
            command(QStringList{u"video-add"_s, VideoSettings::defaultCover()});
        }
        break;
    }
    case AsyncIds::AddSubtitleTrack: {
        loadTracks(getProperty(MpvProperties::self()->TrackList).toList());
    }
    }
}

void MpvItem::onChapterChanged()
{
    if (!finishedLoading() || !PlaybackSettings::skipChapters()) {
        return;
    }

    const QString chaptersToSkip = PlaybackSettings::chaptersToSkip();
    if (m_chaptersModel->rowCount() == 0 || chaptersToSkip == QString()) {
        return;
    }

    const QStringList words = chaptersToSkip.split(u","_s);
    if (words.isEmpty()) {
        return;
    }

    const auto modelIndex = m_chaptersModel->index(chapter(), 0);
    const auto title = m_chaptersModel->data(modelIndex, ChaptersModel::Roles::TitleRole).toString();
    for (const auto &word : words) {
        const auto _word = word.simplified();
        if (title.contains(_word, Qt::CaseInsensitive)) {
            command({u"add"_s, u"chapter"_s, u"1"_s});
            if (PlaybackSettings::showOsdOnSkipChapters()) {
                Q_EMIT osdMessage(i18nc("@info:tooltip osd", "Skipped chapter: %1", title));
            }
            return;
        }
    }
}

void MpvItem::updateTaskbarPlaybackProgress()
{
#if HAVE_DBUS
    if (!GeneralSettings::showTaskbarProgress()) {
        return;
    }
    if (duration() <= 0) {
        return;
    }

    static QList<PlaybackState> activeStates = {PlaybackState::Playing, PlaybackState::Paused};
    bool isProgressVisible = activeStates.contains(playbackState());

    QVariantMap map;
    map.insert(u"progress-visible"_s, isProgressVisible);
    map.insert(u"progress"_s, position() / duration());

    QDBusMessage msg = QDBusMessage::createSignal(u"/org/kde/haruna"_s, u"com.canonical.Unity.LauncherEntry"_s, u"Update"_s);
    msg << u"application://org.kde.haruna.desktop"_s << map;

    QDBusConnection::sessionBus().send(msg);
#endif
}

void MpvItem::saveTimePosition()
{
    // position is saved only for files longer than PlaybackSettings::minDurationToSavePosition()
    if (duration() < PlaybackSettings::minDurationToSavePosition() * 60) {
        return;
    }

    getPropertyAsync(MpvProperties::self()->Position, static_cast<int>(AsyncIds::SavePosition));
}

double MpvItem::loadTimePosition()
{
    auto *const playlistModel = playlistsManager()->activePlaylist()->playlistModel();
    PlaylistItem item{playlistModel->playlist().at(playlistModel->playingItem())};
    auto duration{item.duration};

    if (qFuzzyCompare(duration, 0.0)) {
        QString mimeType = MiscUtils::mimeType(m_currentUrl);
        KFileMetaData::ExtractorCollection exCol;
        QList<KFileMetaData::Extractor *> extractors = exCol.fetchExtractors(mimeType);
        KFileMetaData::SimpleExtractionResult result(m_currentUrl.toLocalFile(), mimeType, KFileMetaData::ExtractionResult::ExtractMetaData);
        if (!extractors.isEmpty()) {
            KFileMetaData::Extractor *ex = extractors.first();
            ex->extract(&result);
            auto properties = result.properties();
            duration = properties.value(KFileMetaData::Property::Duration).toDouble();
        }
    }

    // position for files with a duration lower than
    // PlaybackSettings::minDurationToSavePosition() is not restored
    if (m_currentUrl.isLocalFile() && duration < PlaybackSettings::minDurationToSavePosition() * 60) {
        return 0;
    }

    auto hash = MiscUtils::md5(currentUrl().toString());
    return Database::instance()->playbackPosition(hash);
}

void MpvItem::resetTimePosition()
{
    auto hash = MiscUtils::md5(currentUrl().toString());
    Database::instance()->deletePlaybackPosition(hash);
}

void MpvItem::userCommand(const QString &commandString)
{
    QStringList args = KShell::splitArgs(commandString.simplified());
    command(args);
}

void MpvItem::selectSubtitleTrack()
{
    setProperty(MpvProperties::self()->SubtitleId, u"no"_s);
    if (SubtitlesSettings::autoSelectSubtitles()) {
        const QVariant preferredSubTrack = SubtitlesSettings::preferredTrack();
        setProperty(MpvProperties::self()->SubtitleId, preferredSubTrack == 0 ? u"auto"_s : preferredSubTrack);
        setProperty(MpvProperties::self()->SubtitleLanguage, SubtitlesSettings::preferredLanguage().remove(u" "_s));
    }
}

void MpvItem::addSubtitles(const QString &subtitlesFile)
{
    commandAsync({u"sub-add"_s, subtitlesFile, u"select"_s}, static_cast<int>(MpvItem::AsyncIds::AddSubtitleTrack));
}

void MpvItem::stop()
{
    command({u"stop"_s});
    setPlaybackState(PlaybackState::Stopped);
    setPause(true);
    update();

    auto *const playlistModel = playlistsManager()->activePlaylist();
    playlistModel->setIsPlaying(false);
}

void MpvItem::setTrack(int row, TrackType type)
{
    switch (type) {
    case TrackType::Audio: {
        auto track = m_audioTracksModel->track(row);
        setProperty(MpvProperties::self()->AudioId, track.trackid);
        m_audioTracksModel->setActiveRow(row);
        auto msg = i18nc(
            "@info:tooltip; %1 is the track number, "
            "%2 is a string in the form: `track.title track.language track.codec`",
            "Audio: %1 %2",
            row + 1,
            track.display());
        Q_EMIT osdMessage(msg);
        break;
    }
    case TrackType::Subtitle: {
        auto track = m_subtitleTracksModel->track(row);
        setProperty(MpvProperties::self()->SubtitleId, track.trackid);
        m_subtitleTracksModel->setActiveRow(row);
        if (row == 0) {
            Q_EMIT osdMessage(i18nc("@info:tooltip; osd message when no sub is selected", "Subtitle: none"));
        } else {
            auto msg = i18nc(
                "@info:tooltip; %1 is the track number, "
                "%2 is a string in the form: `track.title track.language track.codec`",
                "Subtitle: %1 %2",
                row,
                track.display());
            Q_EMIT osdMessage(msg);
        }
        break;
    }
    case TrackType::SecondarySubtitle: {
        auto track = m_subtitleTracksModel->track(row);
        setProperty(MpvProperties::self()->SecondarySubtitleId, track.trackid);
        if (row == 0) {
            Q_EMIT osdMessage(i18nc("@info:tooltip; osd message when no sub is selected", "Secondary subtitle: none"));
        } else {
            auto msg = i18nc(
                "@info:tooltip; %1 is the track number, "
                "%2 is a string in the form: `track.title track.language track.codec`",
                "Subtitle: %1 %2",
                row,
                track.display());
            Q_EMIT osdMessage(msg);
        }
        break;
    }
    case TrackType::Video:
    default:
        break;
    }
}

void MpvItem::setNextTrack(TrackType type)
{
    switch (type) {
    case TrackType::Audio: {
        auto nextRow = m_audioTracksModel->nextRow();
        setTrack(nextRow, type);
        break;
    }
    case TrackType::Subtitle: {
        auto nextRow = m_subtitleTracksModel->nextRow();
        setTrack(nextRow, type);
        break;
    }
    case TrackType::SecondarySubtitle:
    case TrackType::Video:
    default:
        break;
    }
}

void MpvItem::setPreviousTrack(TrackType type)
{
    switch (type) {
    case TrackType::Audio: {
        auto previousRow = m_audioTracksModel->previousRow();
        setTrack(previousRow, type);
        break;
    }
    case TrackType::Subtitle: {
        auto previousRow = m_subtitleTracksModel->previousRow();
        setTrack(previousRow, type);
        break;
    }
    case TrackType::SecondarySubtitle:
    case TrackType::Video:
    default:
        break;
    }
}

ChaptersModel *MpvItem::chaptersModel() const
{
    return m_chaptersModel.get();
}

void MpvItem::setChaptersModel(ChaptersModel *_chaptersModel)
{
    if (m_chaptersModel.get() == _chaptersModel) {
        return;
    }
    m_chaptersModel.reset(_chaptersModel);
    Q_EMIT chaptersModelChanged();
}

TracksModel *MpvItem::subtitleTracksModel() const
{
    return m_subtitleTracksModel.get();
}

TracksModel *MpvItem::audioTracksModel() const
{
    return m_audioTracksModel.get();
}

PlaylistsManager *MpvItem::playlistsManager() const
{
    return m_playlistsManager;
}

void MpvItem::setPlaylistsManager(PlaylistsManager *newPlaylistsManager)
{
    if (m_playlistsManager != nullptr) {
        return;
    }
    m_playlistsManager = newPlaylistsManager;

    connect(playlistsManager(), &PlaylistsManager::playingItemChanged, this, [this]() {
        loadFile(playlistsManager()->activeItemPath());
    });

    Q_EMIT playlistsManagerChanged();
}

QString MpvItem::mediaTitle()
{
    return m_mediaTitle;
}

double MpvItem::position()
{
    return m_position;
}

void MpvItem::setPosition(double value)
{
    if (qFuzzyCompare(value, position())) {
        return;
    }
    setPropertyAsync(MpvProperties::self()->Position, value);
}

double MpvItem::remaining()
{
    return m_remaining;
}

double MpvItem::duration()
{
    return m_duration;
}

bool MpvItem::pause()
{
    return m_pause;
}

void MpvItem::setPause(bool value)
{
    if (value == pause()) {
        return;
    }
    setProperty(MpvProperties::self()->Pause, value);
}

int MpvItem::volume()
{
    return m_volume;
}

void MpvItem::setVolume(int value)
{
    if (value == volume()) {
        return;
    }
    setProperty(MpvProperties::self()->Volume, value);
}

int MpvItem::volumeMax()
{
    return m_volumeMax;
}

void MpvItem::setVolumeMax(int value)
{
    if (volumeMax() == value) {
        return;
    }

    setProperty(MpvProperties::self()->VolumeMax, value);
}

bool MpvItem::mute()
{
    return m_mute;
}

void MpvItem::setMute(bool value)
{
    if (value == mute()) {
        return;
    }
    setProperty(MpvProperties::self()->Mute, value);
}

int MpvItem::chapter()
{
    return m_chapter;
}

void MpvItem::setChapter(int value)
{
    if (value == chapter()) {
        return;
    }
    setProperty(MpvProperties::self()->Chapter, value);
}

int MpvItem::audioId()
{
    return m_audioId;
}

void MpvItem::setAudioId(int value)
{
    if (value == audioId()) {
        return;
    }
    setProperty(MpvProperties::self()->AudioId, value);
}

int MpvItem::subtitleId()
{
    return m_subtitleId;
}

void MpvItem::setSubtitleId(int value)
{
    if (value == subtitleId()) {
        return;
    }
    setProperty(MpvProperties::self()->SubtitleId, value);
}

int MpvItem::secondarySubtitleId()
{
    return m_secondarySubtitleId;
}

void MpvItem::setSecondarySubtitleId(int value)
{
    if (value == secondarySubtitleId()) {
        return;
    }
    setProperty(MpvProperties::self()->SecondarySubtitleId, value);
}

QString MpvItem::formattedDuration() const
{
    return m_formattedDuration;
}

QString MpvItem::formattedRemaining() const
{
    return m_formattedRemaining;
}

QString MpvItem::formattedPosition() const
{
    return m_formattedPosition;
}

QUrl MpvItem::currentUrl() const
{
    return m_currentUrl;
}

bool MpvItem::isReady() const
{
    return m_isReady;
}

inline void MpvItem::setIsReady(bool _isReady)
{
    if (m_isReady == _isReady) {
        return;
    }
    m_isReady = _isReady;
    Q_EMIT isReadyChanged();
}

MpvItem::PlaybackState MpvItem::playbackState() const
{
    return m_playbackState;
}

void MpvItem::setPlaybackState(PlaybackState newPlaybackState)
{
    if (m_playbackState == newPlaybackState) {
        return;
    }

    m_playbackState = newPlaybackState;
    Q_EMIT playbackStateChanged();
}

double MpvItem::watchLaterPosition() const
{
    return m_watchLaterPosition;
}

void MpvItem::setWatchLaterPosition(double _watchLaterPosition)
{
    if (qFuzzyCompare(m_watchLaterPosition + 1, _watchLaterPosition + 1)) {
        return;
    }
    m_watchLaterPosition = _watchLaterPosition;
    Q_EMIT watchLaterPositionChanged();
}

bool MpvItem::finishedLoading() const
{
    return m_finishedLoading;
}

void MpvItem::setFinishedLoading(bool _finishedLoading)
{
    if (m_finishedLoading == _finishedLoading) {
        return;
    }
    m_finishedLoading = _finishedLoading;
    Q_EMIT finishedLoadingChanged();
}

double MpvItem::watchPercentage()
{
    return m_watchPercentage;
}

void MpvItem::setWatchPercentage(double value)
{
    if (qFuzzyCompare(m_watchPercentage, value)) {
        return;
    }
    m_watchPercentage = value;
    Q_EMIT watchPercentageChanged();
}

int MpvItem::videoWidth()
{
    return m_videoWidth;
}

int MpvItem::videoHeight()
{
    return m_videoHeight;
}

#include "moc_mpvitem.cpp"
