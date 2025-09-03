/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "_debug.h"

#include <QObject>
#include <QtGlobal>
#include <QOpenGLContext>
#include <QGuiApplication>

#include <QOpenGLFramebufferObject>

#include <QQuickWindow>
#include <QQuickView>
#include <QDateTime>
#include <QStandardPaths>
#include <QDir>

#include "mpvobject.h"
#include "qthelper.h"
#include "track.h"
#include "tracksmodel.h"

void on_mpv_redraw(void *ctx)
{
    MpvObject::on_update(ctx);
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

MpvRenderer::~MpvRenderer()
{}

void MpvRenderer::render()
{
    QOpenGLFramebufferObject *fbo = framebufferObject();
    mpv_opengl_fbo mpfbo{.fbo = static_cast<int>(fbo->handle()), .w = fbo->width(), .h = fbo->height(), .internal_format = 0};
    int flip_y{0};

    mpv_render_param params[] = {
        // Specify the default framebuffer (0) as target. This will
        // render onto the entire screen. If you want to show the video
        // in a smaller rectangle or apply fancy transformations, you'll
        // need to render into a separate FBO and draw it manually.
        {MPV_RENDER_PARAM_OPENGL_FBO, &mpfbo},
        // Flip rendering (needed due to flipped GL coordinate system).
        {MPV_RENDER_PARAM_FLIP_Y, &flip_y},
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
    , mpv{mpv_create()}, mpv_gl(nullptr)
    , m_audioTracksModel(new TracksModel)
    , m_subtitleTracksModel(new TracksModel)
{
    if (!mpv)
        throw std::runtime_error("could not create mpv context");

//    mpv_set_option_string(mpv, "terminal", "yes");
//    mpv_set_option_string(mpv, "msg-level", "all=v");
    setProperty("vo", "libmpv");
    setProperty("hwdec", "auto");

    mpv_observe_property(mpv, 0, "time-pos", MPV_FORMAT_DOUBLE);
    mpv_observe_property(mpv, 0, "time-remaining", MPV_FORMAT_DOUBLE);
    mpv_observe_property(mpv, 0, "duration", MPV_FORMAT_DOUBLE);
    mpv_observe_property(mpv, 0, "volume", MPV_FORMAT_INT64);
    mpv_observe_property(mpv, 0, "pause", MPV_FORMAT_FLAG);
    mpv_observe_property(mpv, 0, "chapter", MPV_FORMAT_INT64);
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
    connect(this, &MpvObject::onUpdate, this, &MpvObject::doUpdate,
            Qt::QueuedConnection);
}

MpvObject::~MpvObject()
{
    // only initialized if something got drawn
    if (mpv_gl) {
        mpv_render_context_free(mpv_gl);
    }

    mpv_terminate_destroy(mpv);
}

void MpvObject::on_update(void *ctx)
{
    MpvObject *self = (MpvObject *)ctx;
    emit self->onUpdate();
}

// connected to onUpdate(); signal makes sure it runs on the GUI thread
void MpvObject::doUpdate()
{
    update();
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
                    m_position = *(double *)prop->data;

                    emit onPositionChanged(m_position);
                }
            } else if (strcmp(prop->name, "time-remaining") == 0) {
                if (prop->format == MPV_FORMAT_DOUBLE) {
                    m_remaining = *(double *)prop->data;

                    emit onRemainingChanged(m_remaining);
                }
            } else if (strcmp(prop->name, "duration") == 0) {
                if (prop->format == MPV_FORMAT_DOUBLE) {
                    m_duration = *(double *)prop->data;

                    emit onDurationChanged(m_duration);
                }
            } else if (strcmp(prop->name, "volume") == 0) {
                if (prop->format == MPV_FORMAT_INT64) {
                    m_volume = *(int *)prop->data;

                    emit onVolumeChanged(m_volume);
                }
            } else if (strcmp(prop->name, "pause") == 0) {
                if (prop->format == MPV_FORMAT_FLAG) {
                    m_pause = *(bool *)prop->data;
                    emit onPauseChanged(m_pause);
                }
            } else if (strcmp(prop->name, "chapter") == 0) {
                if (prop->format == MPV_FORMAT_INT64) {
                    m_chapter = *(int *)prop->data;
                    emit onChapterChanged(m_chapter);
                }
            } else if (strcmp(prop->name, "contrast") == 0) {
                if (prop->format == MPV_FORMAT_INT64) {
                    m_contrast = *(int *)prop->data;
                    emit onContrastChanged(m_contrast);
                }
            } else if (strcmp(prop->name, "brightness") == 0) {
                if (prop->format == MPV_FORMAT_INT64) {
                    m_brightness = *(int *)prop->data;
                    emit onBrightnessChanged(m_brightness);
                }
            } else if (strcmp(prop->name, "gamma") == 0) {
                if (prop->format == MPV_FORMAT_INT64) {
                    m_gamma = *(int *)prop->data;
                    emit onGammaChanged(m_gamma);
                }
            } else if (strcmp(prop->name, "saturation") == 0) {
                if (prop->format == MPV_FORMAT_INT64) {
                    m_saturation = *(int *)prop->data;
                    emit onSaturationChanged(m_saturation);
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

    auto *none = new Track();
    none->setFirst(false);
    none->setSecond(false);
    none->setId(-1);
    none->setTitle("None");
    m_subtitleTracks.insert(0, none);
    m_subtitleTracksModel->setFirstTrack(0);
    m_subtitleTracksModel->setSecondTrack(0);

    QVariant tracks = getProperty("track-list");
    int subIndex = 1;
    int audioIndex = 0;
    for (auto track : tracks.toList()) {
        if (track.toMap()["type"] == "sub") {
            const auto t = track.toMap();
            auto *track = new Track();
            track->setCodec(t["codec"].toString());
            track->setType(t["type"].toString());
            track->setDefaut(t["default"].toBool());
            track->setDependent(t["dependent"].toBool());
            track->setForced(t["forced"].toBool());
            track->setFirst(t["selected"].toBool());
            track->setId(t["id"].toLongLong());
            track->setSrcId(t["src-id"].toLongLong());
            track->setFfIndex(t["ff-index"].toLongLong());
            track->setLang(t["lang"].toString());
            track->setTitle(t["title"].toString());
            track->setIndex(subIndex);
            if (t["selected"].toBool()) {
                m_subtitleTracksModel->setFirstTrack(subIndex);
            }

            m_subtitleTracks.insert(subIndex, track);
            subIndex++;
        }
        if (track.toMap()["type"] == "audio") {
            const auto t = track.toMap();
            auto *track = new Track();
            track->setCodec(t["codec"].toString());
            track->setType(t["type"].toString());
            track->setDefaut(t["default"].toBool());
            track->setDependent(t["dependent"].toBool());
            track->setForced(t["forced"].toBool());
            track->setFirst(t["selected"].toBool());
            track->setId(t["id"].toLongLong());
            track->setSrcId(t["src-id"].toLongLong());
            track->setFfIndex(t["ff-index"].toLongLong());
            track->setLang(t["lang"].toString());
            track->setTitle(t["title"].toString());
            track->setIndex(audioIndex);
            if (t["selected"].toBool()) {
                m_audioTracksModel->setFirstTrack(audioIndex);
            }

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

QVariant MpvObject::command(const QVariant& params)
{
    return mpv::qt::command(mpv, params);
}

int MpvObject::setProperty(const QString& name, const QVariant& value)
{
    return mpv::qt::set_property(mpv, name, value);
}

QVariant MpvObject::getProperty(const QString& name)
{
    emit onUpdate();
    auto value = mpv::qt::get_property(mpv, name);
    return value;
}

QQuickFramebufferObject::Renderer *MpvObject::createRenderer() const
{
    window()->setPersistentSceneGraph(true);
    return new MpvRenderer(const_cast<MpvObject *>(this));
}

QString MpvObject::formatTime(double time)
{
    QDateTime d = QDateTime::fromSecsSinceEpoch(static_cast<qint64>(time)).toUTC();
    QString formattedTime = d.toString("hh:mm:ss");
    return formattedTime;
}
