/*
 * SPDX-FileCopyrightText: 2022 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "mpvcore.h"
#include "mpvrenderer.h"

MpvCore::MpvCore(QQuickItem *parent)
    : QQuickFramebufferObject(parent)
{
    m_mpv = mpv_create();
    if (!m_mpv) {
        qFatal("could not create mpv context");
    }
    if (mpv_initialize(m_mpv) < 0) {
        qFatal("could not initialize mpv context");
    }
}

MpvCore::~MpvCore()
{
    if (m_mpv_gl) {
        mpv_render_context_free(m_mpv_gl);
    }
    mpv_terminate_destroy(m_mpv);
}

QQuickFramebufferObject::Renderer *MpvCore::createRenderer() const
{
    return new MpvRenderer(const_cast<MpvCore *>(this));
}

void MpvCore::mpvEvents(void *ctx)
{
    QMetaObject::invokeMethod(static_cast<MpvCore*>(ctx), &MpvCore::eventHandler, Qt::QueuedConnection);
}

int MpvCore::setProperty(const QString &name, const QVariant &value)
{
    mpv::qt::node_builder node(value);
    return mpv_set_property(m_mpv, name.toUtf8().data(), MPV_FORMAT_NODE, node.node());
}

QVariant MpvCore::getProperty(const QString &name)
{
    mpv_node node;
    int err = mpv_get_property(m_mpv, name.toUtf8().data(), MPV_FORMAT_NODE, &node);
    if (err < 0) {
        return QVariant::fromValue(mpv::qt::ErrorReturn(err));
    }
    mpv::qt::node_autofree f(&node);
    return mpv::qt::node_to_variant(&node);
}

QVariant MpvCore::command(const QVariant &params)
{
    mpv::qt::node_builder node(params);
    mpv_node result;
    int err = mpv_command_node(m_mpv, node.node(), &result);
    if (err < 0) {
        return QVariant::fromValue(mpv::qt::ErrorReturn(err));
    }
    mpv::qt::node_autofree f(&result);
    return mpv::qt::node_to_variant(&result);
}
