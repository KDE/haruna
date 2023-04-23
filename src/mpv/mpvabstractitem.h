/*
 * SPDX-FileCopyrightText: 2022 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef MPVABSTRACTITEM_H
#define MPVABSTRACTITEM_H

#include <QtQuick/QQuickFramebufferObject>
#include <mpv/client.h>
#include <mpv/render_gl.h>

class MpvController;

class MpvAbstractItem : public QQuickFramebufferObject
{
    Q_OBJECT
public:
    MpvAbstractItem(QQuickItem *parent = nullptr);
    ~MpvAbstractItem();

    Renderer *createRenderer() const override;
    Q_INVOKABLE int setProperty(const QString &name, const QVariant &value);
    Q_INVOKABLE QVariant getProperty(const QString &name);
    Q_INVOKABLE QVariant command(const QStringList &params);

    friend class MpvRenderer;

Q_SIGNALS:
    void ready();

protected:
    QThread *m_workerThread{nullptr};
    MpvController *m_mpvController{nullptr};
    mpv_handle *m_mpv{nullptr};
    mpv_render_context *m_mpv_gl{nullptr};
};

#endif // MPVABSTRACTITEM_H
