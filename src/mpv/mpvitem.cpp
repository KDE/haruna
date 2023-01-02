/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "mpvitem.h"
#include "application.h"
#include "audiosettings.h"
#include "generalsettings.h"
#include "global.h"
#include "playbacksettings.h"
#include "playlistitem.h"
#include "subtitlessettings.h"
#include "track.h"
#include "tracksmodel.h"
#include "videosettings.h"
#include "worker.h"

#include <QCryptographicHash>
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QProcess>
#include <QStandardPaths>
#include <QtGlobal>

#include <KLocalizedString>
#include <KShell>
#include <QCommandLineParser>

#if defined(Q_OS_UNIX)
#include "lockmanager.h"
#include "mediaplayer2.h"
#include "mediaplayer2player.h"
#include <QDBusConnection>
#endif

MpvItem::MpvItem(QQuickItem *parent)
    : MpvCore(parent)
    , m_audioTracksModel(new TracksModel)
    , m_subtitleTracksModel(new TracksModel)
    , m_playlistProxyModel(new PlayListProxyModel)
{
    m_playlistModel = new PlayListModel();
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
    mpv_observe_property(m_mpv, 0, "secondary-sid", MPV_FORMAT_INT64);
    mpv_observe_property(m_mpv, 0, "track-list", MPV_FORMAT_NODE);

    initProperties();

    // run user commands
    KSharedConfig::Ptr m_customPropsConfig;
    QString ccConfig = Global::instance()->appConfigFilePath(Global::ConfigFile::CustomCommands);
    m_customPropsConfig = KSharedConfig::openConfig(ccConfig, KConfig::SimpleConfig);
    QStringList groups = m_customPropsConfig->groupList();
    for (const QString &_group : qAsConst((groups))) {
        auto configGroup = m_customPropsConfig->group(_group);
        QString type = configGroup.readEntry("Type", QString());
        bool setOnStartup = configGroup.readEntry("SetOnStartup", true);
        if (type == "startup" && setOnStartup) {
            userCommand(configGroup.readEntry("Command", QString()));
        }
    }

    connect(this, &MpvItem::positionChanged, this, [this]() {
        int pos = getProperty("time-pos").toInt();
        double duration = getProperty("duration").toDouble();
        if (!m_secondsWatched.contains(pos)) {
            m_secondsWatched << pos;
            setWatchPercentage(m_secondsWatched.count() * 100 / duration);
        }
    });

    connect(m_playlistModel, &PlayListModel::playingItemChanged, this, [=]() {
        command(QStringList() << "loadfile" << m_playlistModel->getPath());
    });

#if defined(Q_OS_UNIX)
    // register mpris dbus service
    QString mspris2Name(QStringLiteral("org.mpris.MediaPlayer2.haruna"));
    QDBusConnection::sessionBus().registerService(mspris2Name);
    QDBusConnection::sessionBus().registerObject(QStringLiteral("/org/mpris/MediaPlayer2"), this, QDBusConnection::ExportAdaptors);
    // org.mpris.MediaPlayer2 mpris2 interface
    new MediaPlayer2(this);
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
        command(QStringList() << "add"
                              << "time-pos" << QString::number(offset));
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

    connect(this, &MpvItem::syncConfigValue, Worker::instance(), &Worker::syncConfigValue);
}

void MpvItem::initProperties()
{
    //    setProperty("terminal", "yes");
    //    setProperty("msg-level", "all=v");

    QString hwdec = PlaybackSettings::useHWDecoding() ? PlaybackSettings::hWDecoding() : "no";
    setProperty("hwdec", hwdec);
    setProperty("sub-auto", "exact");
    setProperty("volume-max", "100");
    // set ytdl_path to yt-dlp or fallback to youtube-dl
    setProperty("script-opts", QString("ytdl_hook-ytdl_path=%1").arg(Application::youtubeDlExecutable()));

    setProperty("sub-use-margins", SubtitlesSettings::allowOnBlackBorders() ? "yes" : "no");
    setProperty("sub-ass-force-margins", SubtitlesSettings::allowOnBlackBorders() ? "yes" : "no");
    QCommandLineParser *cmdParser = Application::instance()->parser();
    QString ytdlFormat = PlaybackSettings::ytdlFormat();
    if (cmdParser->isSet(QStringLiteral("ytdl-format-selection"))) {
        ytdlFormat = cmdParser->value(QStringLiteral("ytdl-format-selection"));
    }
    setProperty("ytdl-format", ytdlFormat);

    setProperty("sub-font", SubtitlesSettings::fontFamily());
    setProperty("sub-font-size", SubtitlesSettings::fontSize());
    setProperty("sub-color", SubtitlesSettings::fontColor());
    setProperty("sub-shadow-color", SubtitlesSettings::shadowColor());
    setProperty("sub-shadow-offset", SubtitlesSettings::shadowOffset());
    setProperty("sub-border-color", SubtitlesSettings::borderColor());
    setProperty("sub-border-size", SubtitlesSettings::borderSize());
    setProperty("sub-bold", SubtitlesSettings::isBold());
    setProperty("sub-italic", SubtitlesSettings::isItalic());

    setProperty("screenshot-template", VideoSettings::screenshotTemplate());
    setProperty("screenshot-format", VideoSettings::screenshotFormat());

    setProperty("audio-client-name", "haruna");
    const QVariant preferredAudioTrack = AudioSettings::preferredTrack();
    setProperty("aid", preferredAudioTrack == 0 ? "auto" : preferredAudioTrack);
    setProperty("alang", AudioSettings::preferredLanguage());

    const QVariant preferredSubTrack = SubtitlesSettings::preferredTrack();
    setProperty("sid", preferredSubTrack == 0 ? "auto" : preferredSubTrack);
    setProperty("slang", SubtitlesSettings::preferredLanguage());
    setProperty("sub-file-paths", SubtitlesSettings::subtitlesFolders().join(":"));
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

QString MpvItem::mediaTitle()
{
    return getProperty("media-title").toString();
}
double MpvItem::position()
{
    return getProperty("time-pos").toDouble();
}

void MpvItem::setPosition(double value)
{
    if (value == position()) {
        return;
    }
    setProperty("time-pos", value);
    Q_EMIT positionChanged();
}

double MpvItem::remaining()
{
    return getProperty("time-remaining").toDouble();
}

double MpvItem::duration()
{
    return getProperty("duration").toDouble();
}

bool MpvItem::pause()
{
    return getProperty("pause").toBool();
}

void MpvItem::setPause(bool value)
{
    if (value == pause()) {
        return;
    }
    setProperty("pause", value);
    Q_EMIT pauseChanged();
}

int MpvItem::volume()
{
    return getProperty("volume").toInt();
}

void MpvItem::setVolume(int value)
{
    if (value == volume()) {
        return;
    }
    setProperty("volume", value);
    Q_EMIT volumeChanged();
}

bool MpvItem::mute()
{
    return getProperty("mute").toBool();
}

void MpvItem::setMute(bool value)
{
    if (value == mute()) {
        return;
    }
    setProperty("mute", value);
    Q_EMIT muteChanged();
}

int MpvItem::chapter()
{
    return getProperty("chapter").toInt();
}

void MpvItem::setChapter(int value)
{
    if (value == chapter()) {
        return;
    }
    setProperty("chapter", value);
    Q_EMIT chapterChanged();
}

int MpvItem::audioId()
{
    return getProperty("aid").toInt();
}

void MpvItem::setAudioId(int value)
{
    if (value == audioId()) {
        return;
    }
    setProperty("aid", value);
    Q_EMIT audioIdChanged();
}

int MpvItem::subtitleId()
{
    return getProperty("sid").toInt();
}

void MpvItem::setSubtitleId(int value)
{
    if (value == subtitleId()) {
        return;
    }
    setProperty("sid", value);
    Q_EMIT subtitleIdChanged();
}

int MpvItem::secondarySubtitleId()
{
    return getProperty("secondary-sid").toInt();
}

void MpvItem::setSecondarySubtitleId(int value)
{
    if (value == secondarySubtitleId()) {
        return;
    }
    setProperty("secondary-sid", value);
    Q_EMIT secondarySubtitleIdChanged();
}

double MpvItem::watchPercentage()
{
    return m_watchPercentage;
}

void MpvItem::setWatchPercentage(double value)
{
    if (m_watchPercentage == value) {
        return;
    }
    m_watchPercentage = value;
    Q_EMIT watchPercentageChanged();
}

bool MpvItem::hwDecoding()
{
    if (getProperty("hwdec") == "yes") {
        return true;
    } else {
        return false;
    }
}

void MpvItem::setHWDecoding(bool value)
{
    if (value) {
        setProperty("hwdec", "yes");
    } else {
        setProperty("hwdec", "no");
    }
    Q_EMIT hwDecodingChanged();
}

void MpvItem::loadFile(const QString &file)
{
    command(QStringList() << "loadfile" << file);

    GeneralSettings::setLastPlayedFile(file);
    GeneralSettings::self()->save();
}

void MpvItem::eventHandler()
{
    while (m_mpv) {
        mpv_event *event = mpv_wait_event(m_mpv, 0);
        if (event->event_id == MPV_EVENT_NONE) {
            break;
        }
        switch (event->event_id) {
        case MPV_EVENT_START_FILE: {
            Q_EMIT fileStarted();
            break;
        }
        case MPV_EVENT_FILE_LOADED: {
            Q_EMIT fileLoaded();
            break;
        }
        case MPV_EVENT_END_FILE: {
            auto prop = static_cast<mpv_event_end_file *>(event->data);
            if (prop->reason == MPV_END_FILE_REASON_EOF) {
                Q_EMIT endFile("eof");
            } else if (prop->reason == MPV_END_FILE_REASON_ERROR) {
                Q_EMIT endFile("error");
            }
            break;
        }
        case MPV_EVENT_PROPERTY_CHANGE: {
            mpv_event_property *prop = static_cast<mpv_event_property *>(event->data);

            if (strcmp(prop->name, "time-pos") == 0) {
                if (prop->format == MPV_FORMAT_DOUBLE) {
                    Q_EMIT positionChanged();
                }
            } else if (strcmp(prop->name, "media-title") == 0) {
                if (prop->format == MPV_FORMAT_STRING) {
                    Q_EMIT mediaTitleChanged();
                }
            } else if (strcmp(prop->name, "time-remaining") == 0) {
                if (prop->format == MPV_FORMAT_DOUBLE) {
                    Q_EMIT remainingChanged();
                }
            } else if (strcmp(prop->name, "duration") == 0) {
                if (prop->format == MPV_FORMAT_DOUBLE) {
                    Q_EMIT durationChanged();
                }
            } else if (strcmp(prop->name, "volume") == 0) {
                if (prop->format == MPV_FORMAT_INT64) {
                    Q_EMIT volumeChanged();
                }
            } else if (strcmp(prop->name, "pause") == 0) {
                if (prop->format == MPV_FORMAT_FLAG) {
                    Q_EMIT pauseChanged();
                }
            } else if (strcmp(prop->name, "chapter") == 0) {
                if (prop->format == MPV_FORMAT_INT64) {
                    Q_EMIT chapterChanged();
                }
            } else if (strcmp(prop->name, "aid") == 0) {
                if (prop->format == MPV_FORMAT_INT64) {
                    Q_EMIT audioIdChanged();
                }
            } else if (strcmp(prop->name, "sid") == 0) {
                if (prop->format == MPV_FORMAT_INT64) {
                    Q_EMIT subtitleIdChanged();
                }
            } else if (strcmp(prop->name, "secondary-sid") == 0) {
                if (prop->format == MPV_FORMAT_INT64) {
                    Q_EMIT secondarySubtitleIdChanged();
                }
            } else if (strcmp(prop->name, "track-list") == 0) {
                if (prop->format == MPV_FORMAT_NODE) {
                    loadTracks();
                }
            }
            break;
        }
        default:;
            // Ignore uninteresting or unknown events.
        }
    }
}

void MpvItem::loadTracks()
{
    m_subtitleTracks.clear();
    m_audioTracks.clear();

    auto none = new Track();
    none->setId(0);
    none->setTitle("None");
    m_subtitleTracks.insert(0, none);

    const QList<QVariant> tracks = getProperty("track-list").toList();
    int subIndex = 1;
    int audioIndex = 0;
    for (const auto &track : tracks) {
        const auto trackMap = track.toMap();
        if (trackMap["type"] == "sub") {
            auto t = new Track();
            t->setCodec(trackMap["codec"].toString());
            t->setType(trackMap["type"].toString());
            t->setDefaut(trackMap["default"].toBool());
            t->setDependent(trackMap["dependent"].toBool());
            t->setForced(trackMap["forced"].toBool());
            t->setId(trackMap["id"].toLongLong());
            t->setSrcId(trackMap["src-id"].toLongLong());
            t->setFfIndex(trackMap["ff-index"].toLongLong());
            t->setLang(trackMap["lang"].toString());
            t->setTitle(trackMap["title"].toString());
            t->setIndex(subIndex);

            m_subtitleTracks.insert(subIndex, t);
            subIndex++;
        }
        if (trackMap["type"] == "audio") {
            auto t = new Track();
            t->setCodec(trackMap["codec"].toString());
            t->setType(trackMap["type"].toString());
            t->setDefaut(trackMap["default"].toBool());
            t->setDependent(trackMap["dependent"].toBool());
            t->setForced(trackMap["forced"].toBool());
            t->setId(trackMap["id"].toLongLong());
            t->setSrcId(trackMap["src-id"].toLongLong());
            t->setFfIndex(trackMap["ff-index"].toLongLong());
            t->setLang(trackMap["lang"].toString());
            t->setTitle(trackMap["title"].toString());
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
    if (getProperty("duration").toInt() < PlaybackSettings::minDurationToSavePosition() * 60) {
        return;
    }

    auto hash = md5(getProperty("path").toString());
    auto configPath = Global::instance()->appConfigDirPath();
    configPath.append("/watch-later/").append(hash);

    Q_EMIT syncConfigValue(configPath, "", "TimePosition", getProperty("time-pos"));
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
    if (getProperty("duration").toInt() < PlaybackSettings::minDurationToSavePosition() * 60) {
        return 0;
    }

    auto hash = md5(getProperty("path").toString());
    auto configPath = Global::instance()->appConfigDirPath();
    KConfig *config = new KConfig(configPath.append("/watch-later/").append(hash));
    int pos = config->group("").readEntry("TimePosition", QString::number(0)).toDouble();

    return pos;
}

void MpvItem::resetTimePosition()
{
    auto hash = md5(getProperty("path").toString());
    auto configPath = Global::instance()->appConfigDirPath();
    QFile f(configPath.append("/watch-later/").append(hash));

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

    return QString(md5.toHex());
}

#include "moc_mpvitem.cpp"
