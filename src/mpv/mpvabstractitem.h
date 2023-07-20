/*
 * SPDX-FileCopyrightText: 2022 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef MPVABSTRACTITEM_H
#define MPVABSTRACTITEM_H

#include "mpvcontroller.h"

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
    Q_INVOKABLE void setProperty(const QString &name, const QVariant &value);
    Q_INVOKABLE void setProperty(MpvController::Properties property, const QVariant &value);
    Q_INVOKABLE QVariant getProperty(const QString &name);
    Q_INVOKABLE QVariant getCachedPropertyValue(const QString &property);
    Q_INVOKABLE void command(const QStringList &params);
    Q_INVOKABLE QVariant synchronousCommand(const QStringList &params);
    Q_INVOKABLE QVariant expandText(const QString &text);

    friend class MpvRenderer;

Q_SIGNALS:
    void ready();
    void mpvCommand(const QStringList &params);
    void setMpvProperty(const QString &property, const QVariant &value);
    void setMpvProperty(MpvController::Properties property, const QVariant &value);

protected:
    void cachePropertyValue(const QString &property, const QVariant &value);

    QThread *m_workerThread{nullptr};
    MpvController *m_mpvController{nullptr};
    mpv_handle *m_mpv{nullptr};
    mpv_render_context *m_mpv_gl{nullptr};
    QMap<QString, QVariant> m_propertiesCache;
};

#endif // MPVABSTRACTITEM_H
