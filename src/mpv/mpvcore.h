/*
 * SPDX-FileCopyrightText: 2022 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef MPVCORE_H
#define MPVCORE_H

#include <QtQuick/QQuickFramebufferObject>

#include <mpv/client.h>
#include <mpv/render_gl.h>
#include "mpvqthelper.h"

class MpvCore : public QQuickFramebufferObject
{
    Q_OBJECT
public:
    MpvCore(QQuickItem * parent = nullptr);
    ~MpvCore();
    Renderer *createRenderer() const override;

    friend class MpvRenderer;

    /**
     * Set the given property as mpv_node converted from the QVariant argument.
     *
     * @return mpv error code (<0 on error, >= 0 on success)
     */
    Q_INVOKABLE int setProperty(const QString &name, const QVariant &value);

    /**
     * Return the given property as mpv_node converted to QVariant, or QVariant()
     * on error.
     *
     * @param `name` the property name
     * @return the property value, or an ErrorReturn with the error code
     */
    Q_INVOKABLE QVariant getProperty(const QString &name);

    /**
     * mpv_command_node() equivalent.
     *
     * @param `args` command arguments, with args[0] being the command name as string
     * @return the property value, or an ErrorReturn with the error code
     */
    Q_INVOKABLE QVariant command(const QVariant &params);

    static void mpvEvents(void *ctx);
    virtual void eventHandler() = 0;

signals:
    void ready();

protected:
    mpv_handle *m_mpv {nullptr};
    mpv_render_context *m_mpv_gl {nullptr};

};

#endif // MPVCORE_H
