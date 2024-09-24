/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "_debug.h"
#include "mpvobject.h"
#include "track.h"
#include "tracksmodel.h"

#include <QDir>
#include <QObject>
#include <QOpenGLContext>
#include <QOpenGLFramebufferObject>
#include <QQuickWindow>
#include <QStandardPaths>
#include <QtGlobal>

void on_mpv_redraw(void *ctx)
{
    QMetaObject::invokeMethod(static_cast<MpvObject*>(ctx), "update", Qt::QueuedConnection);
}

static void *get_proc_address_mpv(void *ctx, const char *name)
{
    Q_UNUSED(ctx)

    QOpenGLContext *glctx = QOpenGLContext::currentContext();
    if (!glctx) return nullptr;

    return reinterpret_cast<void *>(glctx->getProcAddress(QByteArray(name)));
}

MpvRenderer::MpvRenderer(MpvObject *new_obj)
    : obj{new_obj}
{}

void MpvRenderer::render()
{
    QOpenGLFramebufferObject *fbo = framebufferObject();
    mpv_opengl_fbo mpfbo;
    mpfbo.fbo = static_cast<int>(fbo->handle());
    mpfbo.w = fbo->width();
    mpfbo.h = fbo->height();
    mpfbo.internal_format = 0;

    mpv_render_param params[] = {
        // Specify the default framebuffer (0) as target. This will
        // render onto the entire screen. If you want to show the video
        // in a smaller rectangle or apply fancy transformations, you'll
        // need to render into a separate FBO and draw it manually.
        {MPV_RENDER_PARAM_OPENGL_FBO, &mpfbo},
        {MPV_RENDER_PARAM_INVALID, nullptr}
    };
    // See render_gl.h on what OpenGL environment mpv expects, and
    // other API details.
    mpv_render_context_render(obj->mpv_gl, params);
}

QOpenGLFramebufferObject * MpvRenderer::createFramebufferObject(const QSize &size)
{
    // init mpv_gl:
    if (!obj->mpv_gl)
    {
        mpv_opengl_init_params gl_init_params{get_proc_address_mpv, nullptr};
        mpv_render_param params[]{
            {MPV_RENDER_PARAM_API_TYPE, const_cast<char *>(MPV_RENDER_API_TYPE_OPENGL)},
            {MPV_RENDER_PARAM_OPENGL_INIT_PARAMS, &gl_init_params},
            {MPV_RENDER_PARAM_INVALID, nullptr}
        };

        if (mpv_render_context_create(&obj->mpv_gl, obj->mpv, params) < 0)
            throw std::runtime_error("failed to initialize mpv GL context");
        mpv_render_context_set_update_callback(obj->mpv_gl, on_mpv_redraw, obj);
        emit obj->ready();
    }

    return QQuickFramebufferObject::Renderer::createFramebufferObject(size);
}

MpvObject::MpvObject(QQuickItem * parent)
    : QQuickFramebufferObject(parent)
    , mpv{mpv_create()}
    , mpv_gl(nullptr)
    , m_audioTracksModel(new TracksModel)
    , m_subtitleTracksModel(new TracksModel)
{
    if (!mpv)
        throw std::runtime_error("could not create mpv context");

//    mpv_set_option_string(mpv, "terminal", "yes");
//    mpv_set_option_string(mpv, "msg-level", "all=v");
    setProperty("vo", "libmpv");
    setProperty("hwdec", "auto");
    setProperty("screenshot-template", "%x/screenshots/%n");

    mpv_observe_property(mpv, 0, "media-title", MPV_FORMAT_STRING);
    mpv_observe_property(mpv, 0, "time-pos", MPV_FORMAT_DOUBLE);
    mpv_observe_property(mpv, 0, "time-remaining", MPV_FORMAT_DOUBLE);
    mpv_observe_property(mpv, 0, "duration", MPV_FORMAT_DOUBLE);
    mpv_observe_property(mpv, 0, "volume", MPV_FORMAT_INT64);
    mpv_observe_property(mpv, 0, "pause", MPV_FORMAT_FLAG);
    mpv_observe_property(mpv, 0, "chapter", MPV_FORMAT_INT64);
    mpv_observe_property(mpv, 0, "aid", MPV_FORMAT_INT64);
    mpv_observe_property(mpv, 0, "sid", MPV_FORMAT_INT64);
    mpv_observe_property(mpv, 0, "secondary-sid", MPV_FORMAT_INT64);
    mpv_observe_property(mpv, 0, "contrast", MPV_FORMAT_INT64);
    mpv_observe_property(mpv, 0, "brightness", MPV_FORMAT_INT64);
    mpv_observe_property(mpv, 0, "gamma", MPV_FORMAT_INT64);
    mpv_observe_property(mpv, 0, "saturation", MPV_FORMAT_INT64);
    setProperty("sub-auto", "exact");

    QString configPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    QString watchLaterPath = configPath.append("/georgefb/watch-later");
    setProperty("watch-later-directory", watchLaterPath);
    QDir watchLaterDir(watchLaterPath);
    if (!watchLaterDir.exists()) {
        QDir().mkdir(watchLaterPath);
    }

    if (mpv_initialize(mpv) < 0)
        throw std::runtime_error("could not initialize mpv context");

    mpv_set_wakeup_callback(mpv, MpvObject::on_mpv_events, this);
}

MpvObject::~MpvObject()
{
    // only initialized if something got drawn
    if (mpv_gl) {
        mpv_render_context_free(mpv_gl);
    }
    mpv_terminate_destroy(mpv);
}

QString MpvObject::mediaTitle()
{
    return getProperty("media-title").toString();
}
double MpvObject::position()
{
    return getProperty("time-pos").toDouble();
}

void MpvObject::setPosition(double value)
{
    if (value == position()) {
        return;
    }
    setProperty("time-pos", value);
    emit positionChanged();
}

double MpvObject::remaining()
{
    return getProperty("time-remaining").toDouble();
}

double MpvObject::duration()
{
    return getProperty("duration").toDouble();
}

bool MpvObject::pause()
{
    return getProperty("pause").toBool();
}

int MpvObject::volume()
{
    return getProperty("volume").toInt();
}

void MpvObject::setVolume(int value)
{
    if (value == volume()) {
        return;
    }
    setProperty("volume", value);
    emit volumeChanged();
}

int MpvObject::chapter()
{
    return getProperty("chapter").toInt();
}

void MpvObject::setChapter(int value)
{
    if (value == chapter()) {
        return;
    }
    setProperty("chapter", value);
    emit chapterChanged();
}

int MpvObject::audioId()
{
    return getProperty("aid").toInt();
}

void MpvObject::setAudioId(int value)
{
    if (value == audioId()) {
        return;
    }
    setProperty("aid", value);
    emit audioIdChanged();
}

int MpvObject::subtitleId()
{
    return getProperty("sid").toInt();
}

void MpvObject::setSubtitleId(int value)
{
    if (value == subtitleId()) {
        return;
    }
    setProperty("sid", value);
    emit subtitleIdChanged();
}

int MpvObject::secondarySubtitleId()
{
    return getProperty("secondary-sid").toInt();
}

void MpvObject::setSecondarySubtitleId(int value)
{
    if (value == secondarySubtitleId()) {
        return;
    }
    setProperty("secondary-sid", value);
    emit secondarySubtitleIdChanged();
}

int MpvObject::contrast()
{
    return getProperty("contrast").toInt();
}

void MpvObject::setContrast(int value)
{
    if (value == contrast()) {
        return;
    }
    setProperty("contrast", value);
    emit contrastChanged();
}

int MpvObject::brightness()
{
    return getProperty("brightness").toInt();
}

void MpvObject::setBrightness(int value)
{
    if (value == brightness()) {
        return;
    }
    setProperty("brightness", value);
    emit brightnessChanged();
}

int MpvObject::gamma()
{
    return getProperty("gamma").toInt();
}

void MpvObject::setGamma(int value)
{
    if (value == gamma()) {
        return;
    }
    setProperty("gamma", value);
    emit gammaChanged();
}

int MpvObject::saturation()
{
    return getProperty("saturation").toInt();
}

void MpvObject::setSaturation(int value)
{
    if (value == saturation()) {
        return;
    }
    setProperty("saturation", value);
    emit saturationChanged();
}


QQuickFramebufferObject::Renderer *MpvObject::createRenderer() const
{
    window()->setPersistentSceneGraph(true);
    return new MpvRenderer(const_cast<MpvObject *>(this));
}

void MpvObject::on_mpv_events(void *ctx)
{
    QMetaObject::invokeMethod(static_cast<MpvObject*>(ctx), "eventHandler", Qt::QueuedConnection);
}

void MpvObject::eventHandler()
{
    while (mpv) {
        mpv_event *event = mpv_wait_event(mpv, 0);
        if (event->event_id == MPV_EVENT_NONE) {
            break;
        }
        switch (event->event_id) {
        case MPV_EVENT_FILE_LOADED: {
            loadTracks();
            emit fileLoaded();
            break;
        }
        case MPV_EVENT_END_FILE: {
            auto prop = (mpv_event_end_file *)event->data;
            if (prop->reason == MPV_END_FILE_REASON_EOF) {
                emit endOfFile();
            }
            break;
        }
        case MPV_EVENT_PROPERTY_CHANGE: {
            mpv_event_property *prop = (mpv_event_property *)event->data;

            if (strcmp(prop->name, "time-pos") == 0) {
                if (prop->format == MPV_FORMAT_DOUBLE) {
                    emit positionChanged();
                }
            } else if (strcmp(prop->name, "media-title") == 0) {
                if (prop->format == MPV_FORMAT_STRING) {
                    emit mediaTitleChanged();
                }
            } else if (strcmp(prop->name, "time-remaining") == 0) {
                if (prop->format == MPV_FORMAT_DOUBLE) {
                    emit remainingChanged();
                }
            } else if (strcmp(prop->name, "duration") == 0) {
                if (prop->format == MPV_FORMAT_DOUBLE) {
                    emit durationChanged();
                }
            } else if (strcmp(prop->name, "volume") == 0) {
                if (prop->format == MPV_FORMAT_INT64) {
                    emit volumeChanged();
                }
            } else if (strcmp(prop->name, "pause") == 0) {
                if (prop->format == MPV_FORMAT_FLAG) {
                    emit pauseChanged();
                }
            } else if (strcmp(prop->name, "chapter") == 0) {
                if (prop->format == MPV_FORMAT_INT64) {
                    emit chapterChanged();
                }
            } else if (strcmp(prop->name, "aid") == 0) {
                if (prop->format == MPV_FORMAT_INT64) {
                    emit audioIdChanged();
                }
            } else if (strcmp(prop->name, "sid") == 0) {
                if (prop->format == MPV_FORMAT_INT64) {
                    emit subtitleIdChanged();
                } else {
                    emit subtitleIdChanged();
                }
            } else if (strcmp(prop->name, "secondary-sid") == 0) {
                if (prop->format == MPV_FORMAT_INT64) {
                    emit secondarySubtitleIdChanged();
                } else {
                    emit secondarySubtitleIdChanged();
                }
            } else if (strcmp(prop->name, "contrast") == 0) {
                if (prop->format == MPV_FORMAT_INT64) {
                    emit contrastChanged();
                }
            } else if (strcmp(prop->name, "brightness") == 0) {
                if (prop->format == MPV_FORMAT_INT64) {
                    emit brightnessChanged();
                }
            } else if (strcmp(prop->name, "gamma") == 0) {
                if (prop->format == MPV_FORMAT_INT64) {
                    emit gammaChanged();
                }
            } else if (strcmp(prop->name, "saturation") == 0) {
                if (prop->format == MPV_FORMAT_INT64) {
                    emit saturationChanged();
                }
            }
            break;
        }
        default: ;
            // Ignore uninteresting or unknown events.
        }
    }
}

void MpvObject::loadTracks()
{
    m_subtitleTracks.clear();
    m_audioTracks.clear();

    auto none = new Track();
    none->setId(0);
    none->setTitle("None");
    m_subtitleTracks.insert(0, none);

    QVariant tracks = getProperty("track-list");
    int subIndex = 1;
    int audioIndex = 0;
    for (const auto &track : tracks.toList()) {
        const auto t = track.toMap();
        if (track.toMap()["type"] == "sub") {
            auto track = new Track();
            track->setCodec(t["codec"].toString());
            track->setType(t["type"].toString());
            track->setDefaut(t["default"].toBool());
            track->setDependent(t["dependent"].toBool());
            track->setForced(t["forced"].toBool());
            track->setId(t["id"].toLongLong());
            track->setSrcId(t["src-id"].toLongLong());
            track->setFfIndex(t["ff-index"].toLongLong());
            track->setLang(t["lang"].toString());
            track->setTitle(t["title"].toString());
            track->setIndex(subIndex);

            m_subtitleTracks.insert(subIndex, track);
            subIndex++;
        }
        if (track.toMap()["type"] == "audio") {
            auto track = new Track();
            track->setCodec(t["codec"].toString());
            track->setType(t["type"].toString());
            track->setDefaut(t["default"].toBool());
            track->setDependent(t["dependent"].toBool());
            track->setForced(t["forced"].toBool());
            track->setId(t["id"].toLongLong());
            track->setSrcId(t["src-id"].toLongLong());
            track->setFfIndex(t["ff-index"].toLongLong());
            track->setLang(t["lang"].toString());
            track->setTitle(t["title"].toString());
            track->setIndex(audioIndex);

            m_audioTracks.insert(audioIndex, track);
            audioIndex++;
        }
    }
    m_subtitleTracksModel->setTracks(m_subtitleTracks);
    m_audioTracksModel->setTracks(m_audioTracks);
}

TracksModel *MpvObject::subtitleTracksModel() const
{
    return m_subtitleTracksModel;
}

TracksModel *MpvObject::audioTracksModel() const
{
    return m_audioTracksModel;
}

int MpvObject::setProperty(const QString &name, const QVariant &value)
{
    return mpv::qt::set_property(mpv, name, value);
}

QVariant MpvObject::getProperty(const QString &name)
{
    auto value = mpv::qt::get_property(mpv, name);
    return value;
}

QVariant MpvObject::command(const QVariant &params)
{
    return mpv::qt::command(mpv, params);
}
