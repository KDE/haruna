/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "mpvitem.h"

#include <QCryptographicHash>
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QProcess>
#include <QStandardPaths>
#include <QTimer>
#include <QtGlobal>
#include <playlistsettings.h>

#include <KLocalizedString>
#include <KShell>
#include <QCommandLineParser>

#include "application.h"
#include "audiosettings.h"
#include "generalsettings.h"
#include "global.h"
#include "informationsettings.h"
#include "mpvcontroller.h"
#include "playbacksettings.h"
#include "subtitlessettings.h"
#include "track.h"
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
    , m_audioTracksModel{new TracksModel}
    , m_subtitleTracksModel{new TracksModel}
    , m_playlistModel{new PlayListModel}
    , m_playlistProxyModel{new PlayListProxyModel}
    , m_chaptersModel{new ChaptersModel}
    , m_watchLaterPath{QString(Global::instance()->appConfigDirPath()).append(QStringLiteral("/watch-later/"))}
{
    m_playlistProxyModel->setSourceModel(m_playlistModel);
    mpv_observe_property(m_mpv, 0, "media-title", MPV_FORMAT_STRING);
    mpv_observe_property(m_mpv, 0, "time-pos", MPV_FORMAT_DOUBLE);
    mpv_observe_property(m_mpv, 0, "time-remaining", MPV_FORMAT_DOUBLE);
    mpv_observe_property(m_mpv, 0, "duration", MPV_FORMAT_DOUBLE);
    mpv_observe_property(m_mpv, 0, "pause", MPV_FORMAT_FLAG);
    mpv_observe_property(m_mpv, 0, "volume", MPV_FORMAT_INT64);
    mpv_observe_property(m_mpv, 0, "volume-max", MPV_FORMAT_INT64);
    mpv_observe_property(m_mpv, 0, "mute", MPV_FORMAT_FLAG);
    mpv_observe_property(m_mpv, 0, "aid", MPV_FORMAT_INT64);
    mpv_observe_property(m_mpv, 0, "sid", MPV_FORMAT_INT64);
    mpv_observe_property(m_mpv, 0, "chapter", MPV_FORMAT_INT64);
    mpv_observe_property(m_mpv, 0, "chapter-list", MPV_FORMAT_NODE);
    mpv_observe_property(m_mpv, 0, "secondary-sid", MPV_FORMAT_INT64);
    mpv_observe_property(m_mpv, 0, "track-list", MPV_FORMAT_NODE);

    initProperties();
    setupConnections();

    auto *timer = new QTimer();
    timer->setInterval(PlaybackSettings::savePositionInterval() * 1000);
    timer->start();

    connect(timer, &QTimer::timeout, this, [=]() {
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

    // run user commands
    KSharedConfig::Ptr m_customPropsConfig;
    QString ccConfig = Global::instance()->appConfigFilePath(Global::ConfigFile::CustomCommands);
    m_customPropsConfig = KSharedConfig::openConfig(ccConfig, KConfig::SimpleConfig);
    QStringList groups = m_customPropsConfig->groupList();
    for (const QString &_group : qAsConst((groups))) {
        auto configGroup = m_customPropsConfig->group(_group);
        QString type = configGroup.readEntry("Type", QString());
        bool setOnStartup = configGroup.readEntry("SetOnStartup", true);
        if (type == QStringLiteral("startup") && setOnStartup) {
            userCommand(configGroup.readEntry("Command", QString()));
        }
    }
}

void MpvItem::initProperties()
{
    //    setProperty(QStringLiteral("terminal"), InformationSettings::mpvLogging());
    //    setProperty(QStringLiteral("msg-level"), QStringLiteral("all=v"));

    QString hwdec = PlaybackSettings::useHWDecoding() ? PlaybackSettings::hWDecoding() : QStringLiteral("no");
    setProperty(MpvController::Properties::HardwareDecoding, hwdec);
    setProperty(MpvController::Properties::VolumeMax, QStringLiteral("100"));

    // set ytdl_path to yt-dlp or fallback to youtube-dl
    setProperty(MpvController::Properties::ScriptOpts, QStringLiteral("ytdl_hook-ytdl_path=%1").arg(Application::youtubeDlExecutable()));
    QCommandLineParser *cmdParser = Application::instance()->parser();
    QString ytdlFormat = PlaybackSettings::ytdlFormat();
    if (cmdParser->isSet(QStringLiteral("ytdl-format-selection"))) {
        ytdlFormat = cmdParser->value(QStringLiteral("ytdl-format-selection"));
    }
    setProperty(MpvController::Properties::YtdlFormat, ytdlFormat);

    setProperty(MpvController::Properties::SubtitleAuto, QStringLiteral("exact"));
    setProperty(MpvController::Properties::SubtitleUseMargins, SubtitlesSettings::allowOnBlackBorders());
    setProperty(MpvController::Properties::SubtitleAssForceMargins, SubtitlesSettings::allowOnBlackBorders());
    setProperty(MpvController::Properties::SubtitleFont, SubtitlesSettings::fontFamily());
    setProperty(MpvController::Properties::SubtitleFontSize, SubtitlesSettings::fontSize());
    setProperty(MpvController::Properties::SubtitleColor, SubtitlesSettings::fontColor());
    setProperty(MpvController::Properties::SubtitleShadowColor, SubtitlesSettings::shadowColor());
    setProperty(MpvController::Properties::SubtitleShadowOffset, SubtitlesSettings::shadowOffset());
    setProperty(MpvController::Properties::SubtitleBorderColor, SubtitlesSettings::borderColor());
    setProperty(MpvController::Properties::SubtitleBorderSize, SubtitlesSettings::borderSize());
    setProperty(MpvController::Properties::SubtitleBold, SubtitlesSettings::isBold());
    setProperty(MpvController::Properties::SubtitleItalic, SubtitlesSettings::isItalic());

    setProperty(MpvController::Properties::ScreenshotTemplate, VideoSettings::screenshotTemplate());
    setProperty(MpvController::Properties::ScreenshotFormat, VideoSettings::screenshotFormat());

    setProperty(MpvController::Properties::AudioClientName, QStringLiteral("haruna"));
    const QVariant preferredAudioTrack = AudioSettings::preferredTrack();
    setProperty(MpvController::Properties::AudioId, preferredAudioTrack == 0 ? QStringLiteral("auto") : preferredAudioTrack);
    setProperty(MpvController::Properties::AudioLanguage, AudioSettings::preferredLanguage());

    const QVariant preferredSubTrack = SubtitlesSettings::preferredTrack();
    setProperty(MpvController::Properties::SubtitleId, preferredSubTrack == 0 ? QStringLiteral("auto") : preferredSubTrack);
    setProperty(MpvController::Properties::SubtitleLanguage, SubtitlesSettings::preferredLanguage());
    setProperty(MpvController::Properties::SubtitleFilePaths, SubtitlesSettings::subtitlesFolders().join(QStringLiteral(":")));
}

void MpvItem::setupConnections()
{
    // clang-format off
    connect(m_mpvController, &MpvController::propertyChanged,
            this, [=](const QString &property, const QVariant &value) {
        onPropertyChanged(m_mpvController->rProperties().value(property), value);
    }, Qt::QueuedConnection);

    connect(m_mpvController, &MpvController::fileStarted,
            this, &MpvItem::fileStarted, Qt::QueuedConnection);

    connect(m_mpvController, &MpvController::fileLoaded,
            this, &MpvItem::fileLoaded, Qt::QueuedConnection);

    connect(m_mpvController, &MpvController::endFile,
            this, &MpvItem::endFile, Qt::QueuedConnection);

    connect(m_mpvController, &MpvController::videoReconfig,
            this, &MpvItem::videoReconfig, Qt::QueuedConnection);

    connect(m_mpvController, &MpvController::setPropertyReply,
            this, &MpvItem::onSetPropertyReply, Qt::QueuedConnection);

    connect(m_mpvController, &MpvController::getPropertyReply,
            this, &MpvItem::onGetPropertyReply, Qt::QueuedConnection);

    connect(this, &MpvItem::currentUrlChanged, this, [=]() {
        setFinishedLoading(false);
    });

    connect(this, &MpvItem::fileLoaded, this, [=]() {
        if (!getProperty(MpvController::Properties::VideoId).toBool()) {
            command(QStringList{QStringLiteral("video-add"), VideoSettings::defaultCover()});
        }

        setWatchLaterPosition(loadTimePosition());

        if (m_playlistModel->rowCount() <= 1 && PlaylistSettings::repeat()) {
            setProperty(MpvController::Properties::LoopFile, QStringLiteral("inf"));
        }

        setProperty(MpvController::Properties::ABLoopA, QStringLiteral("no"));
        setProperty(MpvController::Properties::ABLoopB, QStringLiteral("no"));

        // this is only run when reloading the last file in the playlist
        // due to the playlist repeat setting being turned off
        if (isFileReloaded()) {
            setPause(true);
            setPosition(0);
            setIsFileReloaded(false);
            setFinishedLoading(true);
            return;
        }

        if (PlaybackSettings::seekToLastPosition()) {
            setPause(!PlaybackSettings::playOnResume() && watchLaterPosition() > 0);
            m_mpvController->setPropertyAsync(MpvController::Properties::Position,
                                              watchLaterPosition(),
                                              MpvController::AsyncIds::FinishedLoading);
        }
    });

    connect(this, &MpvItem::positionChanged, this, [this]() {
        int pos = position();
        double duration = getCachedPropertyValue(MpvController::Properties::Duration).toDouble();
        if (!m_secondsWatched.contains(pos)) {
            m_secondsWatched << pos;
            setWatchPercentage(m_secondsWatched.count() * 100 / duration);
        }
    });

    connect(m_playlistModel, &PlayListModel::playingItemChanged, this, [=]() {
        loadFile(m_playlistModel->getPath());
    });

    connect(this, &MpvItem::chapterListChanged, this, [=]() {
        const auto chapters = getCachedPropertyValue(MpvController::Properties::ChapterList);
        QList<Chapter> chaptersList;
        for (const auto &chapter : chapters.toList()) {
            Chapter c = {
                .title = chapter.toMap()[QStringLiteral("title")].toString(),
                .startTime = chapter.toMap()[QStringLiteral("time")].toDouble(),
            };
            chaptersList.append(c);
        }
        m_chaptersModel->setChapters(chaptersList);
    });


#if defined(Q_OS_UNIX)
    // register mpris dbus service
    QString mspris2Name(QStringLiteral("org.mpris.MediaPlayer2.haruna"));
    QDBusConnection::sessionBus().registerService(mspris2Name);
    QDBusConnection::sessionBus().registerObject(QStringLiteral("/org/mpris/MediaPlayer2"), this, QDBusConnection::ExportAdaptors);
    // org.mpris.MediaPlayer2 mpris2 interface
    auto mp2 = new MediaPlayer2(this);
    connect(mp2, &MediaPlayer2::raise, this, &MpvItem::raise);
    auto mp2Player = new MediaPlayer2Player(this);
    mp2Player->setMpv(this);
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
        command(QStringList() << QStringLiteral("add") << QStringLiteral("time-pos") << QString::number(offset));
    });
    connect(mp2Player, &MediaPlayer2Player::openUri, this, [=](const QString &uri) {
        Q_EMIT openUri(uri);
    });
#endif

#if defined(Q_OS_UNIX)
    auto lockManager = new LockManager(this);
    connect(this, &MpvItem::pauseChanged, this, [=]() {
        if (pause()) {
            lockManager->setInhibitionOff();
        } else {
            lockManager->setInhibitionOn();
        }
    });
#endif

    connect(this, &MpvItem::syncConfigValue, Worker::instance(), &Worker::syncConfigValue, Qt::QueuedConnection);
    // clang-format on
}

void MpvItem::onPropertyChanged(MpvController::Properties property, const QVariant &value)
{
    switch (property) {
    case MpvController::Properties::MediaTitle:
        cachePropertyValue(property, value);
        Q_EMIT mediaTitleChanged();
        break;

    case MpvController::Properties::Position:
        cachePropertyValue(property, value);
        m_formattedPosition = Application::formatTime(value.toDouble());
        Q_EMIT positionChanged();
        break;

    case MpvController::Properties::Remaining:
        cachePropertyValue(property, value);
        m_formattedRemaining = Application::formatTime(value.toDouble());
        Q_EMIT remainingChanged();
        break;

    case MpvController::Properties::Duration:
        cachePropertyValue(property, value);
        m_formattedDuration = Application::formatTime(value.toDouble());
        Q_EMIT durationChanged();
        break;

    case MpvController::Properties::Pause:
        cachePropertyValue(property, value);
        Q_EMIT pauseChanged();
        break;

    case MpvController::Properties::Volume:
        cachePropertyValue(property, value);
        Q_EMIT volumeChanged();
        break;

    case MpvController::Properties::VolumeMax:
        cachePropertyValue(property, value);
        Q_EMIT volumeMaxChanged();
        break;

    case MpvController::Properties::Mute:
        cachePropertyValue(property, value);
        Q_EMIT muteChanged();
        break;

    case MpvController::Properties::Chapter:
        cachePropertyValue(property, value);
        Q_EMIT chapterChanged();
        break;

    case MpvController::Properties::ChapterList:
        cachePropertyValue(property, value);
        Q_EMIT chapterListChanged();
        break;

    case MpvController::Properties::AudioId:
        cachePropertyValue(property, value);
        Q_EMIT audioIdChanged();
        break;

    case MpvController::Properties::SubtitleId:
        cachePropertyValue(property, value);
        Q_EMIT subtitleIdChanged();
        break;

    case MpvController::Properties::SecondarySubtitleId:
        cachePropertyValue(property, value);
        Q_EMIT secondarySubtitleIdChanged();
        break;

    case MpvController::Properties::TrackList:
        loadTracks();
        break;
    }
}

void MpvItem::loadFile(const QString &file)
{
    auto url = QUrl::fromUserInput(file);
    if (m_currentUrl != url) {
        m_currentUrl = url;
        Q_EMIT currentUrlChanged();
    }

    command(QStringList() << QStringLiteral("loadfile") << m_currentUrl.toString());

    GeneralSettings::setLastPlayedFile(m_currentUrl.toString());
    GeneralSettings::self()->save();
}

void MpvItem::loadTracks()
{
    m_subtitleTracks.clear();
    m_audioTracks.clear();

    auto none = new Track();
    none->setId(0);
    none->setTitle(i18nc("@action The \"None\" subtitle track is used to clear/unset selected track", "None"));
    m_subtitleTracks.insert(0, none);

    const QList<QVariant> tracks = getProperty(MpvController::Properties::TrackList).toList();
    int subIndex = 1;
    int audioIndex = 0;
    for (const auto &track : tracks) {
        const auto trackMap = track.toMap();
        if (trackMap[QStringLiteral("type")] == QStringLiteral("sub")) {
            auto t = new Track();
            t->setCodec(trackMap[QStringLiteral("codec")].toString());
            t->setType(trackMap[QStringLiteral("type")].toString());
            t->setDefaut(trackMap[QStringLiteral("default")].toBool());
            t->setDependent(trackMap[QStringLiteral("dependent")].toBool());
            t->setForced(trackMap[QStringLiteral("forced")].toBool());
            t->setId(trackMap[QStringLiteral("id")].toLongLong());
            t->setSrcId(trackMap[QStringLiteral("src-id")].toLongLong());
            t->setFfIndex(trackMap[QStringLiteral("ff-index")].toLongLong());
            t->setLang(trackMap[QStringLiteral("lang")].toString());
            t->setTitle(trackMap[QStringLiteral("title")].toString());
            t->setIndex(subIndex);

            m_subtitleTracks.insert(subIndex, t);
            subIndex++;
        }
        if (trackMap[QStringLiteral("type")] == QStringLiteral("audio")) {
            auto t = new Track();

            t->setCodec(trackMap[QStringLiteral("codec")].toString());
            t->setType(trackMap[QStringLiteral("type")].toString());
            t->setDefaut(trackMap[QStringLiteral("default")].toBool());
            t->setDependent(trackMap[QStringLiteral("dependent")].toBool());
            t->setForced(trackMap[QStringLiteral("forced")].toBool());
            t->setId(trackMap[QStringLiteral("id")].toLongLong());
            t->setSrcId(trackMap[QStringLiteral("src-id")].toLongLong());
            t->setFfIndex(trackMap[QStringLiteral("ff-index")].toLongLong());
            t->setLang(trackMap[QStringLiteral("lang")].toString());
            t->setTitle(trackMap[QStringLiteral("title")].toString());
            t->setIndex(audioIndex);

            m_audioTracks.insert(audioIndex, t);
            audioIndex++;
        }
    }
    m_subtitleTracksModel->setTracks(m_subtitleTracks);
    m_audioTracksModel->setTracks(m_audioTracks);

    Q_EMIT audioTracksModelChanged();
    Q_EMIT subtitleTracksModelChanged();
}

void MpvItem::onSetPropertyReply(MpvController::AsyncIds id)
{
    switch (static_cast<MpvController::AsyncIds>(id)) {
    case MpvController::AsyncIds::FinishedLoading:
        setFinishedLoading(true);
        break;
    }
}

void MpvItem::onGetPropertyReply(const QVariant &value, MpvController::AsyncIds id)
{
    switch (static_cast<MpvController::AsyncIds>(id)) {
    case MpvController::AsyncIds::SavePosition:
        auto hash = md5(currentUrl().toLocalFile());
        auto watchLaterConfig = QString(m_watchLaterPath).append(hash);
        Q_EMIT syncConfigValue(watchLaterConfig, QString(), QStringLiteral("TimePosition"), value);
        break;
    }
}

void MpvItem::saveTimePosition()
{
    // saving position is disabled
    if (PlaybackSettings::minDurationToSavePosition() == -1) {
        return;
    }
    // position is saved only for files longer than PlaybackSettings::minDurationToSavePosition()
    if (getCachedPropertyValue(MpvController::Properties::Duration).toInt() < PlaybackSettings::minDurationToSavePosition() * 60) {
        return;
    }

    m_mpvController->getPropertyAsync(MpvController::Properties::Position, MpvController::AsyncIds::SavePosition);
}

double MpvItem::loadTimePosition()
{
    // saving position is disabled
    if (PlaybackSettings::minDurationToSavePosition() == -1) {
        return 0;
    }
    // position is saved only for files longer than PlaybackSettings::minDurationToSavePosition()
    // but there can be cases when there is a saved position for files lower than minDurationToSavePosition()
    // when minDurationToSavePosition() was increased after position was already saved
    auto duration = getProperty(MpvController::Properties::Duration).toInt();
    if (duration < PlaybackSettings::minDurationToSavePosition() * 60) {
        return 0;
    }

    auto hash = md5(currentUrl().toLocalFile());
    auto watchLaterConfig = QString(m_watchLaterPath).append(hash);
    KConfig config(watchLaterConfig);
    auto pos = config.group("").readEntry("TimePosition", QString::number(0)).toDouble();

    return pos;
}

void MpvItem::resetTimePosition()
{
    auto hash = md5(currentUrl().toLocalFile());
    auto watchLaterConfig = QString(m_watchLaterPath).append(hash);
    QFile f(watchLaterConfig);

    if (f.exists()) {
        f.remove();
    }
    f.close();
}

void MpvItem::userCommand(const QString &commandString)
{
    QStringList args = KShell::splitArgs(commandString);
    command(args);
}

QString MpvItem::md5(const QString &str)
{
    auto md5 = QCryptographicHash::hash((str.toUtf8()), QCryptographicHash::Md5);

    return QString::fromUtf8(md5.toHex());
}

PlayListModel *MpvItem::playlistModel()
{
    return m_playlistModel;
}

void MpvItem::setPlaylistModel(PlayListModel *model)
{
    m_playlistModel = model;
}

PlayListProxyModel *MpvItem::playlistProxyModel()
{
    return m_playlistProxyModel;
}

void MpvItem::setPlaylistProxyModel(PlayListProxyModel *model)
{
    m_playlistProxyModel = model;
}

ChaptersModel *MpvItem::chaptersModel() const
{
    return m_chaptersModel;
}

void MpvItem::setChaptersModel(ChaptersModel *_chaptersModel)
{
    if (m_chaptersModel == _chaptersModel) {
        return;
    }
    m_chaptersModel = _chaptersModel;
    Q_EMIT chaptersModelChanged();
}

TracksModel *MpvItem::subtitleTracksModel() const
{
    return m_subtitleTracksModel;
}

TracksModel *MpvItem::audioTracksModel() const
{
    return m_audioTracksModel;
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
    return getCachedPropertyValue(MpvController::Properties::MediaTitle).toString();
}

double MpvItem::position()
{
    return getCachedPropertyValue(MpvController::Properties::Position).toDouble();
}

void MpvItem::setPosition(double value)
{
    if (qFuzzyCompare(value, position())) {
        return;
    }
    setProperty(MpvController::Properties::Position, value);
}

double MpvItem::remaining()
{
    return getCachedPropertyValue(MpvController::Properties::Remaining).toDouble();
}

double MpvItem::duration()
{
    return getCachedPropertyValue(MpvController::Properties::Duration).toDouble();
}

bool MpvItem::pause()
{
    return getCachedPropertyValue(MpvController::Properties::Pause).toBool();
}

void MpvItem::setPause(bool value)
{
    if (value == pause()) {
        return;
    }
    setProperty(MpvController::Properties::Pause, value);
}

int MpvItem::volume()
{
    return getCachedPropertyValue(MpvController::Properties::Volume).toInt();
}

void MpvItem::setVolume(int value)
{
    if (value == volume()) {
        return;
    }
    setProperty(MpvController::Properties::Volume, value);
}

int MpvItem::volumeMax()
{
    return getCachedPropertyValue(MpvController::Properties::VolumeMax).toInt();
}

void MpvItem::setVolumeMax(int value)
{
    if (volumeMax() == value) {
        return;
    }

    setProperty(MpvController::Properties::VolumeMax, value);
}

bool MpvItem::mute()
{
    return getCachedPropertyValue(MpvController::Properties::Mute).toBool();
}

void MpvItem::setMute(bool value)
{
    if (value == mute()) {
        return;
    }
    setProperty(MpvController::Properties::Mute, value);
}

int MpvItem::chapter()
{
    return getCachedPropertyValue(MpvController::Properties::Chapter).toInt();
}

void MpvItem::setChapter(int value)
{
    if (value == chapter()) {
        return;
    }
    setProperty(MpvController::Properties::Chapter, value);
}

int MpvItem::audioId()
{
    return getCachedPropertyValue(MpvController::Properties::AudioId).toInt();
}

void MpvItem::setAudioId(int value)
{
    if (value == audioId()) {
        return;
    }
    setProperty(MpvController::Properties::AudioId, value);
}

int MpvItem::subtitleId()
{
    return getCachedPropertyValue(MpvController::Properties::SubtitleId).toInt();
}

void MpvItem::setSubtitleId(int value)
{
    if (value == subtitleId()) {
        return;
    }
    setProperty(MpvController::Properties::SubtitleId, value);
}

int MpvItem::secondarySubtitleId()
{
    return getCachedPropertyValue(MpvController::Properties::SecondarySubtitleId).toInt();
}

void MpvItem::setSecondarySubtitleId(int value)
{
    if (value == secondarySubtitleId()) {
        return;
    }
    setProperty(MpvController::Properties::SecondarySubtitleId, value);
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

#include "moc_mpvitem.cpp"
