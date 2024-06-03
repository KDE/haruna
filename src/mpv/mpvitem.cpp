/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "mpvitem.h"

#include <QCommandLineParser>
#include <QCryptographicHash>
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
#include "generalsettings.h"
#include "global.h"
#include "mpvproperties.h"
#include "playbacksettings.h"
#include "playlistmodel.h"
#include "playlistsettings.h"
#include "subtitlessettings.h"
#include "tracksmodel.h"
#include "videosettings.h"
#include "worker.h"

#if defined(Q_OS_UNIX)
#include "lockmanager.h"
#include "mediaplayer2.h"
#include "mediaplayer2player.h"
#include <QDBusConnection>
#endif

MpvItem::MpvItem(QQuickItem *parent)
    : MpvAbstractItem(parent)
    , m_audioTracksModel{std::make_unique<TracksModel>()}
    , m_subtitleTracksModel{std::make_unique<TracksModel>()}
    , m_playlistModel{std::make_unique<PlaylistModel>()}
    , m_playlistProxyModel{std::make_unique<PlaylistProxyModel>()}
    , m_chaptersModel{std::make_unique<ChaptersModel>()}
    , m_watchLaterPath{QString(Global::instance()->appConfigDirPath()).append(QStringLiteral("/watch-later/"))}
    , m_saveTimePositionTimer{std::make_unique<QTimer>()}
{
    m_playlistProxyModel->setSourceModel(m_playlistModel.get());
    Q_EMIT observeProperty(MpvProperties::self()->MediaTitle, MPV_FORMAT_STRING);
    Q_EMIT observeProperty(MpvProperties::self()->Position, MPV_FORMAT_DOUBLE);
    Q_EMIT observeProperty(MpvProperties::self()->Remaining, MPV_FORMAT_DOUBLE);
    Q_EMIT observeProperty(MpvProperties::self()->Duration, MPV_FORMAT_DOUBLE);
    Q_EMIT observeProperty(MpvProperties::self()->Pause, MPV_FORMAT_FLAG);
    Q_EMIT observeProperty(MpvProperties::self()->Volume, MPV_FORMAT_INT64);
    Q_EMIT observeProperty(MpvProperties::self()->VolumeMax, MPV_FORMAT_INT64);
    Q_EMIT observeProperty(MpvProperties::self()->Mute, MPV_FORMAT_FLAG);
    Q_EMIT observeProperty(MpvProperties::self()->AudioId, MPV_FORMAT_INT64);
    Q_EMIT observeProperty(MpvProperties::self()->SubtitleId, MPV_FORMAT_INT64);
    Q_EMIT observeProperty(MpvProperties::self()->Width, MPV_FORMAT_NODE);
    Q_EMIT observeProperty(MpvProperties::self()->Height, MPV_FORMAT_NODE);
    Q_EMIT observeProperty(MpvProperties::self()->SecondarySubtitleId, MPV_FORMAT_INT64);
    Q_EMIT observeProperty(MpvProperties::self()->Chapter, MPV_FORMAT_INT64);
    Q_EMIT observeProperty(MpvProperties::self()->ChapterList, MPV_FORMAT_NODE);
    Q_EMIT observeProperty(MpvProperties::self()->TracksCount, MPV_FORMAT_NODE);

    setupConnections();
    initProperties();

    m_saveTimePositionTimer->setInterval(PlaybackSettings::savePositionInterval() * 1000);
    m_saveTimePositionTimer->start();

    connect(m_saveTimePositionTimer.get(), &QTimer::timeout, this, [=]() {
        if (finishedLoading() && duration() > 0 && !pause()) {
            if (position() < duration() - 10) {
                saveTimePosition();
            } else {
                resetTimePosition();
            }
        }
    });

    connect(QApplication::instance(), &QApplication::aboutToQuit, this, [=]() {
        if (position() < duration() - 10) {
            saveTimePosition();
        } else {
            resetTimePosition();
        }
    });

    connect(this, &MpvAbstractItem::ready, this, &MpvItem::onReady);

    // run user commands
    KSharedConfig::Ptr m_customPropsConfig;
    QString ccConfig = Global::instance()->appConfigFilePath(Global::ConfigFile::CustomCommands);
    m_customPropsConfig = KSharedConfig::openConfig(ccConfig, KConfig::SimpleConfig);
    QStringList groups = m_customPropsConfig->groupList();
    for (const QString &_group : std::as_const(groups)) {
        auto configGroup = m_customPropsConfig->group(_group);
        QString type = configGroup.readEntry("Type", QString());
        bool setOnStartup = configGroup.readEntry("SetOnStartup", true);
        if (type == QStringLiteral("startup") && setOnStartup) {
            userCommand(configGroup.readEntry("Command", QString()));
        }
    }
}

MpvItem::~MpvItem()
{
}

void MpvItem::initProperties()
{
    //    setProperty(QStringLiteral("terminal"), InformationSettings::mpvLogging());
    //    setProperty(QStringLiteral("msg-level"), QStringLiteral("all=v"));

    Q_EMIT setProperty(MpvProperties::self()->VO, QStringLiteral("libmpv"));

    QString hwdec = PlaybackSettings::useHWDecoding() ? PlaybackSettings::hWDecoding() : QStringLiteral("no");
    Q_EMIT setProperty(MpvProperties::self()->HardwareDecoding, hwdec);
    Q_EMIT setProperty(MpvProperties::self()->Volume, AudioSettings::volume());
    Q_EMIT setProperty(MpvProperties::self()->VolumeMax, 100);

    // set ytdl_path to yt-dlp or fallback to youtube-dl
    Q_EMIT setProperty(MpvProperties::self()->ScriptOpts, QStringLiteral("ytdl_hook-ytdl_path=%1").arg(Application::youtubeDlExecutable()));
    QCommandLineParser *cmdParser = Application::instance()->parser();
    QString ytdlFormat = PlaybackSettings::ytdlFormat();
    if (cmdParser->isSet(QStringLiteral("ytdl-format-selection"))) {
        ytdlFormat = cmdParser->value(QStringLiteral("ytdl-format-selection"));
    }
    Q_EMIT setProperty(MpvProperties::self()->YtdlFormat, ytdlFormat);

    Q_EMIT setProperty(MpvProperties::self()->SubtitleAuto, QStringLiteral("exact"));
    Q_EMIT setProperty(MpvProperties::self()->SubtitleUseMargins, SubtitlesSettings::allowOnBlackBorders());
    Q_EMIT setProperty(MpvProperties::self()->SubtitleAssForceMargins, SubtitlesSettings::allowOnBlackBorders());
    Q_EMIT setProperty(MpvProperties::self()->SubtitleFont, SubtitlesSettings::fontFamily());
    Q_EMIT setProperty(MpvProperties::self()->SubtitleFontSize, SubtitlesSettings::fontSize());
    Q_EMIT setProperty(MpvProperties::self()->SubtitleColor, SubtitlesSettings::fontColor());
    Q_EMIT setProperty(MpvProperties::self()->SubtitleShadowColor, SubtitlesSettings::shadowColor());
    Q_EMIT setProperty(MpvProperties::self()->SubtitleShadowOffset, SubtitlesSettings::shadowOffset());
    Q_EMIT setProperty(MpvProperties::self()->SubtitleBorderColor, SubtitlesSettings::borderColor());
    Q_EMIT setProperty(MpvProperties::self()->SubtitleBorderSize, SubtitlesSettings::borderSize());
    Q_EMIT setProperty(MpvProperties::self()->SubtitleBold, SubtitlesSettings::isBold());
    Q_EMIT setProperty(MpvProperties::self()->SubtitleItalic, SubtitlesSettings::isItalic());

    Q_EMIT setProperty(MpvProperties::self()->ScreenshotTemplate, VideoSettings::screenshotTemplate());
    Q_EMIT setProperty(MpvProperties::self()->ScreenshotFormat, VideoSettings::screenshotFormat());

    Q_EMIT setProperty(MpvProperties::self()->AudioClientName, QStringLiteral("haruna"));
    const QVariant preferredAudioTrack = AudioSettings::preferredTrack();
    Q_EMIT setProperty(MpvProperties::self()->AudioId, preferredAudioTrack == 0 ? QStringLiteral("auto") : preferredAudioTrack);
    Q_EMIT setProperty(MpvProperties::self()->AudioLanguage, AudioSettings::preferredLanguage().remove(QStringLiteral(" ")));

    const QVariant preferredSubTrack = SubtitlesSettings::preferredTrack();
    Q_EMIT setProperty(MpvProperties::self()->SubtitleId, preferredSubTrack == 0 ? QStringLiteral("auto") : preferredSubTrack);
    Q_EMIT setProperty(MpvProperties::self()->SubtitleLanguage, SubtitlesSettings::preferredLanguage().remove(QStringLiteral(" ")));
    Q_EMIT setProperty(MpvProperties::self()->SubtitleFilePaths, SubtitlesSettings::subtitlesFolders().join(QStringLiteral(":")));
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

    connect(mpvController(), &MpvController::videoReconfig,
            this, &MpvItem::videoReconfig, Qt::QueuedConnection);

    connect(mpvController(), &MpvController::asyncReply,
            this, &MpvItem::onAsyncReply, Qt::QueuedConnection);

    connect(this, &MpvItem::currentUrlChanged, this, [=]() {
        setFinishedLoading(false);
    });

    connect(mpvController(), &MpvController::fileLoaded, this, [=]() {
        getPropertyAsync(MpvProperties::self()->ChapterList, static_cast<int>(AsyncIds::ChapterList));
        getPropertyAsync(MpvProperties::self()->VideoId, static_cast<int>(AsyncIds::VideoId));
        getPropertyAsync(MpvProperties::self()->TrackList, static_cast<int>(AsyncIds::TrackList));

        if (m_playlistModel->rowCount() <= 1 && PlaylistSettings::repeat()) {
            Q_EMIT setProperty(MpvProperties::self()->LoopFile, QStringLiteral("inf"));
        }

        Q_EMIT setProperty(MpvProperties::self()->ABLoopA, QStringLiteral("no"));
        Q_EMIT setProperty(MpvProperties::self()->ABLoopB, QStringLiteral("no"));

        // this is only run when reloading the last file in the playlist
        // due to the playlist repeat setting being turned off
        if (isFileReloaded()) {
            setPause(true);
            setPosition(0);
            setIsFileReloaded(false);
            setFinishedLoading(true);
            return;
        }

        setFinishedLoading(true);
        Q_EMIT fileLoaded();

    }, Qt::QueuedConnection);

    connect(this, &MpvItem::positionChanged, this, [this]() {
        auto pos = static_cast<int>(position());
        if (!m_secondsWatched.contains(pos)) {
            m_secondsWatched << pos;
            if (m_duration != 0) {
                setWatchPercentage(m_secondsWatched.count() * 100 / m_duration);
            }
        }
    });

    connect(this, &MpvItem::chapterChanged,
            this, &MpvItem::onChapterChanged);

    connect(m_playlistModel.get(), &PlaylistModel::playingItemChanged, this, [=]() {
        loadFile(m_playlistModel->m_playlist[m_playlistModel->m_playingItem].url.toString());
    });


#if HAVE_DBUS
    // register mpris dbus service
    QString mspris2Name(QStringLiteral("org.mpris.MediaPlayer2.haruna"));
    QDBusConnection::sessionBus().registerService(mspris2Name);
    QDBusConnection::sessionBus().registerObject(QStringLiteral("/org/mpris/MediaPlayer2"), this, QDBusConnection::ExportAdaptors);
    // org.mpris.MediaPlayer2 mpris2 interface
    auto mp2 = new MediaPlayer2(this);
    connect(mp2, &MediaPlayer2::raise, this, &MpvItem::raise);
    auto mp2Player = new MediaPlayer2Player(this);
    connect(mp2Player, &MediaPlayer2Player::playpause, this, [=]() {
        setPause(!pause());
    });
    connect(mp2Player, &MediaPlayer2Player::play, this, [=]() {
        setPause(false);
    });
    connect(mp2Player, &MediaPlayer2Player::pause, this, [=]() {
        setPause(true);
    });
    connect(mp2Player, &MediaPlayer2Player::stop, this, [=]() {
        setPosition(0);
        setPause(true);
    });
    connect(mp2Player, &MediaPlayer2Player::next, this, [=]() {
        Q_EMIT playNext();
    });
    connect(mp2Player, &MediaPlayer2Player::previous, this, [=]() {
        Q_EMIT playPrevious();
    });
    connect(mp2Player, &MediaPlayer2Player::seek, this, [=](int offset) {
        Q_EMIT command(QStringList() << QStringLiteral("add") << QStringLiteral("time-pos") << QString::number(offset));
    });
    connect(mp2Player, &MediaPlayer2Player::openUri, this, [=](const QString &uri) {
        Q_EMIT openUri(uri);
    });
#endif

#if defined(Q_OS_UNIX)
    connect(this, &MpvItem::pauseChanged, this, [=]() {
        static LockManager lockManager;
        if (pause()) {
            lockManager.setInhibitionOff();
        } else {
            lockManager.setInhibitionOn();
        }
    });
#endif

    connect(this, &MpvItem::syncConfigValue, Worker::instance(), &Worker::syncConfigValue, Qt::QueuedConnection);
    // clang-format on
}

void MpvItem::onEndFile(const QString &reason)
{
    if (reason == u"error"_qs) {
        if (playlistModel()->rowCount() == 0) {
            return;
        }

        const auto index = playlistProxyModel()->index(playlistProxyModel()->getPlayingItem(), 0);
        const auto title = playlistModel()->data(playlistProxyModel()->mapToSource(index), PlaylistModel::TitleRole);

        Q_EMIT osdMessage(i18nc("@info:tooltip", "Could not play: %1", title.toString()));
    }

    if (playlistProxyModel()->getPlayingItem() + 1 < playlistModel()->rowCount()) {
        playlistProxyModel()->playNext();
    } else {
        // Last file in playlist
        if (PlaylistSettings::repeat()) {
            playlistProxyModel()->setPlayingItem(0);
        } else {
            setIsFileReloaded(true);
            playlistProxyModel()->setPlayingItem(playlistProxyModel()->getPlayingItem());
        }
    }
}

void MpvItem::onReady()
{
    setIsReady(true);

    QUrl url{Application::instance()->url(0)};
    if (!url.isEmpty() && url.isValid()) {
        playlistModel()->addItem(Application::instance()->url(0), PlaylistModel::Clear);
        Q_EMIT addToRecentFiles(url);
    } else {
        if (PlaybackSettings::openLastPlayedFile()) {
            // if both lastPlaylist and lastPlayedFile are set the playlist is loaded
            // and the lastPlayedFile is searched in the playlist
            if (!GeneralSettings::lastPlaylist().isEmpty()) {
                playlistModel()->addItem(GeneralSettings::lastPlaylist(), PlaylistModel::Clear);
            } else {
                playlistModel()->addItem(GeneralSettings::lastPlayedFile(), PlaylistModel::Clear);
            }
        }
    }
}

void MpvItem::onPropertyChanged(const QString &property, const QVariant &value)
{
    if (property == MpvProperties::self()->MediaTitle) {
        m_mediaTitle = value.toString();
        Q_EMIT mediaTitleChanged();

    } else if (property == MpvProperties::self()->Position) {
        m_position = value.toDouble();
        m_formattedPosition = Application::formatTime(m_position);
        Q_EMIT positionChanged();

    } else if (property == MpvProperties::self()->Remaining) {
        m_remaining = value.toDouble();
        m_formattedRemaining = Application::formatTime(m_remaining);
        Q_EMIT remainingChanged();

    } else if (property == MpvProperties::self()->Duration) {
        m_duration = value.toDouble();
        m_formattedDuration = Application::formatTime(m_duration);
        Q_EMIT durationChanged();

    } else if (property == MpvProperties::self()->Pause) {
        m_pause = value.toBool();
        Q_EMIT pauseChanged();

    } else if (property == MpvProperties::self()->Volume) {
        m_volume = value.toInt();
        Q_EMIT volumeChanged();

        AudioSettings::setVolume(m_volume);
        AudioSettings::self()->save();

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

    } else if (property == MpvProperties::self()->Width) {
        m_videoWidth = value.toInt();
        Q_EMIT videoWidthChanged();

    } else if (property == MpvProperties::self()->Height) {
        m_videoHeight = value.toInt();
        Q_EMIT videoHeightChanged();

    } else if (property == MpvProperties::self()->TracksCount) {
        loadTracks(getProperty(MpvProperties::self()->TrackList).toList());
        Q_EMIT videoHeightChanged();
    }
}

void MpvItem::loadFile(const QString &file)
{
    auto url = QUrl::fromUserInput(file);
    if (m_currentUrl != url) {
        m_currentUrl = url;
        Q_EMIT currentUrlChanged();
    }

    // store the mute property so it can be restored after loading file
    auto mute = m_mute;
    // mute to avoid popping sound while loading files
    setPropertyBlocking(MpvProperties::self()->Mute, true);
    setPropertyBlocking(MpvProperties::self()->Pause, false);
    setWatchLaterPosition(loadTimePosition());
    if (PlaybackSettings::seekToLastPosition()) {
        setPropertyBlocking(MpvProperties::self()->Pause, !PlaybackSettings::playOnResume() && watchLaterPosition() > 1);
        setPropertyBlocking(u"start"_qs, QVariant(u"+"_qs + QString::number(m_watchLaterPosition)));
    }
    Q_EMIT command(QStringList() << QStringLiteral("loadfile") << m_currentUrl.toString());
    setPropertyBlocking(MpvProperties::self()->Mute, mute);

    GeneralSettings::setLastPlayedFile(m_currentUrl.toString());
    GeneralSettings::self()->save();
}

void MpvItem::loadTracks(QList<QVariant> tracks)
{
    m_subtitleTracks.clear();
    m_audioTracks.clear();

    QMap<QString, QVariant> none = {
        {QStringLiteral("id"), QVariant(0)},
        {QStringLiteral("title"), QVariant(i18nc("@action The \"None\" subtitle track is used to clear/unset selected track", "None"))},
    };
    m_subtitleTracks.append(none);

    for (const auto &track : tracks) {
        const auto trackMap = track.toMap();
        if (trackMap[QStringLiteral("type")] == QStringLiteral("sub")) {
            m_subtitleTracks.append(track);
        }
        if (trackMap[QStringLiteral("type")] == QStringLiteral("audio")) {
            m_audioTracks.append(track);
        }
    }
    m_subtitleTracksModel->setTracks(std::move(m_subtitleTracks));
    m_audioTracksModel->setTracks(std::move(m_audioTracks));

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
        auto hash = md5(currentUrl().toString());
        auto watchLaterConfig = QString(m_watchLaterPath).append(hash);
        Q_EMIT syncConfigValue(watchLaterConfig, QString(), QStringLiteral("TimePosition"), data.toString());
        break;
    }
    case AsyncIds::Screenshot: {
        if (event.error < 0) {
            Q_EMIT osdMessage(i18nc("@info:tooltip osd", "Screenshot failed"));
            break;
        }
        auto filename = data.toMap().value(QStringLiteral("filename")).toString();
        if (filename.isEmpty()) {
            Q_EMIT osdMessage(i18nc("@info:tooltip osd", "Screenshot taken"));
        } else {
            Q_EMIT osdMessage(i18nc("@info:tooltip osd", "Screenshot: %1", filename));
        }
        break;
    }
    case AsyncIds::TrackList: {
        loadTracks(data.toList());
        break;
    }
    case AsyncIds::ChapterList: {
        m_chaptersList = data.toList();
        QList<Chapter> chaptersList;
        for (const auto &chapter : std::as_const(m_chaptersList)) {
            Chapter c;
            c.title = chapter.toMap()[QStringLiteral("title")].toString();
            c.startTime = chapter.toMap()[QStringLiteral("time")].toDouble();
            chaptersList.append(c);
        }
        m_chaptersModel->setChapters(chaptersList);
        break;
    }
    case AsyncIds::VideoId: {
        if (!data.toBool()) {
            // there's no video track
            // either because the file is an audio file or the video track can't be decoded
            auto mimeType = Application::mimeType(currentUrl());
            if (mimeType.startsWith(u"video/"_qs)) {
                auto errMsg = i18nc("Error message when video can't be decoded/played",
                                    "No video track detected, most likely the video track can't be decoded/played due to missing codecs");
                Q_EMIT Application::instance()->error(errMsg);
                break;
            }
            Q_EMIT command(QStringList{QStringLiteral("video-add"), VideoSettings::defaultCover()});
        }
        break;
    }
    }
}

void MpvItem::onChapterChanged()
{
    if (!finishedLoading() || !PlaybackSettings::skipChapters()) {
        return;
    }

    const QString chaptersToSkip = PlaybackSettings::chaptersToSkip();
    if (m_chaptersList.count() == 0 || chaptersToSkip == QString()) {
        return;
    }

    const QStringList words = chaptersToSkip.split(QStringLiteral(","));
    auto ch = m_chaptersList.value(chapter()).toMap();
    auto title = ch.value(QStringLiteral("title")).toString();
    for (int i = 0; i < words.count(); ++i) {
        QString word = words.at(i).toLower().simplified();
        if (!ch.isEmpty() && title.contains(word, Qt::CaseInsensitive)) {
            Q_EMIT command({QStringLiteral("add"), QStringLiteral("chapter"), QStringLiteral("1")});
            if (PlaybackSettings::showOsdOnSkipChapters()) {
                Q_EMIT osdMessage(i18nc("@info:tooltip osd", "Skipped chapter: %1", title));
            }
            return;
        }
    }
}

void MpvItem::saveTimePosition()
{
    // saving position is disabled
    if (PlaybackSettings::minDurationToSavePosition() == -1) {
        return;
    }
    // position is saved only for files longer than PlaybackSettings::minDurationToSavePosition()
    if (getProperty(MpvProperties::self()->Duration).toInt() < PlaybackSettings::minDurationToSavePosition() * 60) {
        return;
    }

    getPropertyAsync(MpvProperties::self()->Position, static_cast<int>(AsyncIds::SavePosition));
}

double MpvItem::loadTimePosition()
{
    // saving position is disabled
    if (PlaybackSettings::minDurationToSavePosition() == -1) {
        return 0;
    }
    PlaylistItem item{m_playlistModel->m_playlist[m_playlistModel->m_playingItem]};
    auto duration{item.duration};

    if (qFuzzyCompare(duration, 0.0)) {
        QString mimeType = Application::mimeType(m_currentUrl);
        KFileMetaData::ExtractorCollection exCol;
        QList<KFileMetaData::Extractor *> extractors = exCol.fetchExtractors(mimeType);
        KFileMetaData::SimpleExtractionResult result(m_currentUrl.toLocalFile(), mimeType, KFileMetaData::ExtractionResult::ExtractMetaData);
        if (extractors.size() > 0) {
            KFileMetaData::Extractor *ex = extractors.first();
            ex->extract(&result);
            auto properties = result.properties();
            duration = properties.value(KFileMetaData::Property::Duration).toDouble();
        }
    }

    // position for files with a duration lower than
    // PlaybackSettings::minDurationToSavePosition() is not restored
    if (duration < PlaybackSettings::minDurationToSavePosition() * 60) {
        return 0;
    }

    auto hash = md5(currentUrl().toString());
    auto watchLaterConfig = QString(m_watchLaterPath).append(hash);
    KConfig config(watchLaterConfig);
    auto pos = config.group(QString()).readEntry("TimePosition", QString::number(0)).toDouble();

    return pos;
}

void MpvItem::resetTimePosition()
{
    auto hash = md5(currentUrl().toString());
    auto watchLaterConfig = QString(m_watchLaterPath).append(hash);
    QFile f(watchLaterConfig);

    if (f.exists()) {
        f.remove();
    }
    f.close();
}

void MpvItem::userCommand(const QString &commandString)
{
    QStringList args = KShell::splitArgs(commandString.simplified());
    Q_EMIT command(args);
}

QString MpvItem::md5(const QString &str)
{
    auto md5 = QCryptographicHash::hash((str.toUtf8()), QCryptographicHash::Md5);

    return QString::fromUtf8(md5.toHex());
}

PlaylistModel *MpvItem::playlistModel()
{
    return m_playlistModel.get();
}

void MpvItem::setPlaylistModel(PlaylistModel *model)
{
    m_playlistModel.reset(model);
}

PlaylistProxyModel *MpvItem::playlistProxyModel()
{
    return m_playlistProxyModel.get();
}

void MpvItem::setPlaylistProxyModel(PlaylistProxyModel *model)
{
    m_playlistProxyModel.reset(model);
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

bool MpvItem::isFileReloaded() const
{
    return m_isFileReloaded;
}

void MpvItem::setIsFileReloaded(bool _isFileReloaded)
{
    if (m_isFileReloaded == _isFileReloaded) {
        return;
    }
    m_isFileReloaded = _isFileReloaded;
    Q_EMIT isFileReloadedChanged();
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
    Q_EMIT setProperty(MpvProperties::self()->Position, value);
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
    Q_EMIT setProperty(MpvProperties::self()->Pause, value);
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
    Q_EMIT setProperty(MpvProperties::self()->Volume, value);
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

    Q_EMIT setProperty(MpvProperties::self()->VolumeMax, value);
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
    Q_EMIT setProperty(MpvProperties::self()->Mute, value);
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
    Q_EMIT setProperty(MpvProperties::self()->Chapter, value);
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
    Q_EMIT setProperty(MpvProperties::self()->AudioId, value);
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
    Q_EMIT setProperty(MpvProperties::self()->SubtitleId, value);
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
    Q_EMIT setProperty(MpvProperties::self()->SecondarySubtitleId, value);
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
