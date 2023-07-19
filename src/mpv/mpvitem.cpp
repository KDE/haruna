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
{
    m_playlistProxyModel->setSourceModel(m_playlistModel);
    mpv_observe_property(m_mpv, 0, "media-title", MPV_FORMAT_STRING);
    mpv_observe_property(m_mpv, 0, "time-pos", MPV_FORMAT_DOUBLE);
    mpv_observe_property(m_mpv, 0, "time-remaining", MPV_FORMAT_DOUBLE);
    mpv_observe_property(m_mpv, 0, "duration", MPV_FORMAT_DOUBLE);
    mpv_observe_property(m_mpv, 0, "pause", MPV_FORMAT_FLAG);
    mpv_observe_property(m_mpv, 0, "volume", MPV_FORMAT_INT64);
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
        if (duration() > 0 && !pause()) {
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
    setProperty(QStringLiteral("hwdec"), hwdec);
    setProperty(QStringLiteral("sub-auto"), QStringLiteral("exact"));
    setProperty(QStringLiteral("volume-max"), QStringLiteral("100"));
    // set ytdl_path to yt-dlp or fallback to youtube-dl
    setProperty(QStringLiteral("script-opts"), QStringLiteral("ytdl_hook-ytdl_path=%1").arg(Application::youtubeDlExecutable()));

    setProperty(QStringLiteral("sub-use-margins"), SubtitlesSettings::allowOnBlackBorders() ? QStringLiteral("yes") : QStringLiteral("no"));
    setProperty(QStringLiteral("sub-ass-force-margins"), SubtitlesSettings::allowOnBlackBorders() ? QStringLiteral("yes") : QStringLiteral("no"));
    QCommandLineParser *cmdParser = Application::instance()->parser();
    QString ytdlFormat = PlaybackSettings::ytdlFormat();
    if (cmdParser->isSet(QStringLiteral("ytdl-format-selection"))) {
        ytdlFormat = cmdParser->value(QStringLiteral("ytdl-format-selection"));
    }
    setProperty(QStringLiteral("ytdl-format"), ytdlFormat);

    setProperty(QStringLiteral("sub-font"), SubtitlesSettings::fontFamily());
    setProperty(QStringLiteral("sub-font-size"), SubtitlesSettings::fontSize());
    setProperty(QStringLiteral("sub-color"), SubtitlesSettings::fontColor());
    setProperty(QStringLiteral("sub-shadow-color"), SubtitlesSettings::shadowColor());
    setProperty(QStringLiteral("sub-shadow-offset"), SubtitlesSettings::shadowOffset());
    setProperty(QStringLiteral("sub-border-color"), SubtitlesSettings::borderColor());
    setProperty(QStringLiteral("sub-border-size"), SubtitlesSettings::borderSize());
    setProperty(QStringLiteral("sub-bold"), SubtitlesSettings::isBold());
    setProperty(QStringLiteral("sub-italic"), SubtitlesSettings::isItalic());

    setProperty(QStringLiteral("screenshot-template"), VideoSettings::screenshotTemplate());
    setProperty(QStringLiteral("screenshot-format"), VideoSettings::screenshotFormat());

    setProperty(QStringLiteral("audio-client-name"), QStringLiteral("haruna"));
    const QVariant preferredAudioTrack = AudioSettings::preferredTrack();
    setProperty(QStringLiteral("aid"), preferredAudioTrack == 0 ? QStringLiteral("auto") : preferredAudioTrack);
    setProperty(QStringLiteral("alang"), AudioSettings::preferredLanguage());

    const QVariant preferredSubTrack = SubtitlesSettings::preferredTrack();
    setProperty(QStringLiteral("sid"), preferredSubTrack == 0 ? QStringLiteral("auto") : preferredSubTrack);
    setProperty(QStringLiteral("slang"), SubtitlesSettings::preferredLanguage());
    setProperty(QStringLiteral("sub-file-paths"), SubtitlesSettings::subtitlesFolders().join(QStringLiteral(":")));
}

void MpvItem::setupConnections()
{
    // clang-format off
    connect(m_mpvController, &MpvController::propertyChanged,
            this, &MpvItem::onPropertyChanged, Qt::QueuedConnection);

    connect(m_mpvController, &MpvController::fileStarted,
            this, &MpvItem::fileStarted, Qt::QueuedConnection);

    connect(m_mpvController, &MpvController::fileLoaded,
            this, &MpvItem::fileLoaded, Qt::QueuedConnection);

    connect(m_mpvController, &MpvController::endFile,
            this, &MpvItem::endFile, Qt::QueuedConnection);

    connect(m_mpvController, &MpvController::videoReconfig,
            this, &MpvItem::videoReconfig, Qt::QueuedConnection);

    connect(this, &MpvItem::fileLoaded, this, [=]() {
        if (!getProperty(QStringLiteral("vid")).toBool()) {
            command(QStringList{QStringLiteral("video-add"), VideoSettings::defaultCover()});
        }

        setWatchLaterPosition(loadTimePosition());

        if (m_playlistModel->rowCount() <= 1 && PlaylistSettings::repeat()) {
            setProperty(QStringLiteral("loop-file"), QStringLiteral("inf"));
        }

        setProperty(QStringLiteral("ab-loop-a"), QStringLiteral("no"));
        setProperty(QStringLiteral("ab-loop-b"), QStringLiteral("no"));

        // this is only run when reloading the last file in the playlist
        // due to the playlist repeat setting being turned off
        if (isFileReloaded()) {
            setPause(true);
            setPosition(0);
            setIsFileReloaded(false);
            return;
        }

        if (PlaybackSettings::seekToLastPosition()) {
            setPause(!PlaybackSettings::playOnResume() && watchLaterPosition() > 0);
            setPosition(watchLaterPosition());
        }
    });

    connect(this, &MpvItem::positionChanged, this, [this]() {
        int pos = position();
        double duration = getCachedPropertyValue(QStringLiteral("duration")).toDouble();
        if (!m_secondsWatched.contains(pos)) {
            m_secondsWatched << pos;
            setWatchPercentage(m_secondsWatched.count() * 100 / duration);
        }
    });

    connect(m_playlistModel, &PlayListModel::playingItemChanged, this, [=]() {
        loadFile(m_playlistModel->getPath());
    });

    connect(this, &MpvItem::chapterListChanged, this, [=]() {
        const auto chapters = getCachedPropertyValue(QStringLiteral("chapter-list"));
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
    return getCachedPropertyValue(QStringLiteral("media-title")).toString();
}

double MpvItem::position()
{
    return getCachedPropertyValue(QStringLiteral("time-pos")).toDouble();
}

void MpvItem::setPosition(double value)
{
    if (qFuzzyCompare(value, position())) {
        return;
    }
    Q_EMIT setMpvProperty(QStringLiteral("time-pos"), value);
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

double MpvItem::remaining()
{
    return getCachedPropertyValue(QStringLiteral("time-remaining")).toDouble();
}

double MpvItem::duration()
{
    return getCachedPropertyValue(QStringLiteral("duration")).toDouble();
}

bool MpvItem::pause()
{
    return getCachedPropertyValue(QStringLiteral("pause")).toBool();
}

void MpvItem::setPause(bool value)
{
    if (value == pause()) {
        return;
    }
    Q_EMIT setMpvProperty(QStringLiteral("pause"), value);
}

int MpvItem::volume()
{
    return getCachedPropertyValue(QStringLiteral("volume")).toInt();
}

void MpvItem::setVolume(int value)
{
    if (value == volume()) {
        return;
    }
    Q_EMIT setMpvProperty(QStringLiteral("volume"), value);
}

bool MpvItem::mute()
{
    return getCachedPropertyValue(QStringLiteral("mute")).toBool();
}

void MpvItem::setMute(bool value)
{
    if (value == mute()) {
        return;
    }
    Q_EMIT setMpvProperty(QStringLiteral("mute"), value);
}

int MpvItem::chapter()
{
    return getCachedPropertyValue(QStringLiteral("chapter")).toInt();
}

void MpvItem::setChapter(int value)
{
    if (value == chapter()) {
        return;
    }
    Q_EMIT setMpvProperty(QStringLiteral("chapter"), value);
}

int MpvItem::audioId()
{
    return getCachedPropertyValue(QStringLiteral("aid")).toInt();
}

void MpvItem::setAudioId(int value)
{
    if (value == audioId()) {
        return;
    }
    Q_EMIT setMpvProperty(QStringLiteral("aid"), value);
}

int MpvItem::subtitleId()
{
    return getCachedPropertyValue(QStringLiteral("sid")).toInt();
}

void MpvItem::setSubtitleId(int value)
{
    if (value == subtitleId()) {
        return;
    }
    Q_EMIT setMpvProperty(QStringLiteral("sid"), value);
}

int MpvItem::secondarySubtitleId()
{
    return getCachedPropertyValue(QStringLiteral("secondary-sid")).toInt();
}

void MpvItem::setSecondarySubtitleId(int value)
{
    if (value == secondarySubtitleId()) {
        return;
    }
    Q_EMIT setMpvProperty(QStringLiteral("secondary-sid"), value);
}

void MpvItem::onPropertyChanged(const QString &property, const QVariant &value)
{
    if (property == QStringLiteral("media-title")) {
        cachePropertyValue(property, value);
        Q_EMIT mediaTitleChanged();

    } else if (property == QStringLiteral("time-pos")) {
        cachePropertyValue(property, value);
        m_formattedPosition = Application::formatTime(value.toDouble());
        Q_EMIT positionChanged();

    } else if (property == QStringLiteral("time-remaining")) {
        cachePropertyValue(property, value);
        m_formattedRemaining = Application::formatTime(value.toDouble());
        Q_EMIT remainingChanged();

    } else if (property == QStringLiteral("duration")) {
        cachePropertyValue(property, value);
        m_formattedDuration = Application::formatTime(value.toDouble());
        Q_EMIT durationChanged();

    } else if (property == QStringLiteral("pause")) {
        cachePropertyValue(property, value);
        Q_EMIT pauseChanged();

    } else if (property == QStringLiteral("volume")) {
        cachePropertyValue(property, value);
        Q_EMIT volumeChanged();

    } else if (property == QStringLiteral("mute")) {
        cachePropertyValue(property, value);
        Q_EMIT muteChanged();

    } else if (property == QStringLiteral("chapter")) {
        cachePropertyValue(property, value);
        Q_EMIT chapterChanged();

    } else if (property == QStringLiteral("chapter-list")) {
        cachePropertyValue(property, value);
        Q_EMIT chapterListChanged();

    } else if (property == QStringLiteral("aid")) {
        cachePropertyValue(property, value);
        Q_EMIT audioIdChanged();

    } else if (property == QStringLiteral("sid")) {
        cachePropertyValue(property, value);
        Q_EMIT subtitleIdChanged();

    } else if (property == QStringLiteral("secondary-sid")) {
        cachePropertyValue(property, value);
        Q_EMIT secondarySubtitleIdChanged();

    } else if (property == QStringLiteral("track-list")) {
        loadTracks();

    } else {
        // nothing
    }
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

    const QList<QVariant> tracks = getProperty(QStringLiteral("track-list")).toList();
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

TracksModel *MpvItem::subtitleTracksModel() const
{
    return m_subtitleTracksModel;
}

TracksModel *MpvItem::audioTracksModel() const
{
    return m_audioTracksModel;
}

void MpvItem::saveTimePosition()
{
    // saving position is disabled
    if (PlaybackSettings::minDurationToSavePosition() == -1) {
        return;
    }
    // position is saved only for files longer than PlaybackSettings::minDurationToSavePosition()
    if (getCachedPropertyValue(QStringLiteral("duration")).toInt() < PlaybackSettings::minDurationToSavePosition() * 60) {
        return;
    }

    auto hash = md5(getProperty(QStringLiteral("path")).toString());
    auto configPath = Global::instance()->appConfigDirPath();
    configPath.append(QStringLiteral("/watch-later/")).append(hash);

    Q_EMIT syncConfigValue(configPath, QString(), QStringLiteral("TimePosition"), getCachedPropertyValue(QStringLiteral("time-pos")));
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
    if (getProperty(QStringLiteral("duration")).toInt() < PlaybackSettings::minDurationToSavePosition() * 60) {
        return 0;
    }

    auto hash = md5(getProperty(QStringLiteral("path")).toString());
    auto configPath = Global::instance()->appConfigDirPath();
    KConfig *config = new KConfig(configPath.append(QStringLiteral("/watch-later/")).append(hash));
    auto pos = config->group("").readEntry("TimePosition", QString::number(0)).toDouble();

    return pos;
}

void MpvItem::resetTimePosition()
{
    auto hash = md5(getProperty(QStringLiteral("path")).toString());
    auto configPath = Global::instance()->appConfigDirPath();
    QFile f(configPath.append(QStringLiteral("/watch-later/")).append(hash));

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

#include "moc_mpvitem.cpp"
