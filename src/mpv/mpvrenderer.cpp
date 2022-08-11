/*
 * SPDX-FileCopyrightText: 2022 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "mpvrenderer.h"

#include <QQuickWindow>
#include <QOpenGLContext>
#include <QOpenGLFramebufferObject>
#include <QGuiApplication>

#if defined(Q_OS_UNIX) && !defined(Q_OS_DARWIN)
#include <QtX11Extras/QX11Info>
#include <qpa/qplatformnativeinterface.h>
#endif

static void *get_proc_address_mpv(void *ctx, const char *name)
{
    Q_UNUSED(ctx)

    QOpenGLContext *glctx = QOpenGLContext::currentContext();
    if (!glctx) return nullptr;

    return reinterpret_cast<void *>(glctx->getProcAddress(QByteArray(name)));
}

void on_mpv_redraw(void *ctx)
{
    QMetaObject::invokeMethod(static_cast<MpvItem*>(ctx), &MpvItem::update, Qt::QueuedConnection);
}

MpvRenderer::MpvRenderer(MpvCore *new_obj)
    : m_mpv_core{new_obj}
{
    m_mpv_core->window()->setPersistentOpenGLContext(true);
    m_mpv_core->window()->setPersistentSceneGraph(true);
}

void MpvRenderer::render()
{
    m_mpv_core->window()->resetOpenGLState();

    QOpenGLFramebufferObject *fbo = framebufferObject();
    mpv_opengl_fbo mpfbo;
    mpfbo.fbo = static_cast<int>(fbo->handle());
    mpfbo.w = fbo->width();
    mpfbo.h = fbo->height();
    mpfbo.internal_format = 0;

    int flip_y{0};

    mpv_render_param params[] = {
        // Specify the default framebuffer (0) as target. This will
        // render onto the entire screen. If you want to show the video
        // in a smaller rectangle or apply fancy transformations, you'll
        // need to render into a separate FBO and draw it manually.
        {MPV_RENDER_PARAM_OPENGL_FBO, &mpfbo},
        {MPV_RENDER_PARAM_FLIP_Y, &flip_y},
        {MPV_RENDER_PARAM_INVALID, nullptr}
    };
    // See render_gl.h on what OpenGL environment mpv expects, and
    // other API details.
    mpv_render_context_render(m_mpv_core->m_mpv_gl, params);

    m_mpv_core->window()->resetOpenGLState();
}

QOpenGLFramebufferObject *MpvRenderer::createFramebufferObject(const QSize &size)
{
    // init mpv_gl:
    if (!m_mpv_core->m_mpv_gl)
    {
#if MPV_CLIENT_API_VERSION < MPV_MAKE_VERSION(2, 0)
        mpv_opengl_init_params gl_init_params{get_proc_address_mpv, nullptr, nullptr};
#else
        mpv_opengl_init_params gl_init_params{get_proc_address_mpv, nullptr};
#endif

        mpv_render_param display{MPV_RENDER_PARAM_INVALID, nullptr};
#if defined(Q_OS_UNIX) && !defined(Q_OS_DARWIN)
        if(QX11Info::isPlatformX11()) {
            display.type = MPV_RENDER_PARAM_X11_DISPLAY;
            display.data = QX11Info::display();
        }
        if(QGuiApplication::platformName() == QStringLiteral("wayland")) {
            display.type = MPV_RENDER_PARAM_WL_DISPLAY;
            display.data = (struct wl_display*)QGuiApplication::platformNativeInterface()
                    ->nativeResourceForWindow("display", nullptr);
        }
#endif
        mpv_render_param params[]{
            {MPV_RENDER_PARAM_API_TYPE, const_cast<char *>(MPV_RENDER_API_TYPE_OPENGL)},
            {MPV_RENDER_PARAM_OPENGL_INIT_PARAMS, &gl_init_params},
            display,
            {MPV_RENDER_PARAM_INVALID, nullptr}
        };

        if (mpv_render_context_create(&m_mpv_core->m_mpv_gl, m_mpv_core->m_mpv, params) < 0) {
            qFatal("failed to initialize mpv GL context");
        }

        mpv_render_context_set_update_callback(m_mpv_core->m_mpv_gl, on_mpv_redraw, m_mpv_core);
        Q_EMIT m_mpv_core->ready();
    }

    return QQuickFramebufferObject::Renderer::createFramebufferObject(size);
}
