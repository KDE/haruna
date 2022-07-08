#ifndef MPVRENDERER_H
#define MPVRENDERER_H

#include "mpvobject.h"

class MpvRenderer : public QQuickFramebufferObject::Renderer
{
public:
    MpvRenderer(MpvObject *new_obj);
    ~MpvRenderer() = default;

    MpvObject *obj;

    // This function is called when a new FBO is needed.
    // This happens on the initial frame.
    QOpenGLFramebufferObject * createFramebufferObject(const QSize &size) override;

    void render() override;
};

#endif // MPVRENDERER_H
