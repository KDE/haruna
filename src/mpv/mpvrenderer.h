/*
 * SPDX-FileCopyrightText: 2022 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef MPVRENDERER_H
#define MPVRENDERER_H

#include <QtQuick/QQuickFramebufferObject>

#include "mpvcore.h"

class MpvRenderer : public QQuickFramebufferObject::Renderer
{
public:
    explicit MpvRenderer(MpvAbstractItem *new_obj);
    ~MpvRenderer() = default;

    MpvAbstractItem *m_mpvAbstractItem;

    // This function is called when a new FBO is needed.
    // This happens on the initial frame.
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override;

    void render() override;
};

#endif // MPVRENDERER_H
