/*
 * SPDX-FileCopyrightText: 2022 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "mpvabstractitem.h"

#include <QThread>

#include "mpvcontroller.h"
#include "mpvrenderer.h"

MpvAbstractItem::MpvAbstractItem(QQuickItem *parent)
    : QQuickFramebufferObject(parent)
    , m_workerThread{new QThread}
    , m_mpvController{new MpvController}
{
    m_workerThread->start();
    m_mpvController->moveToThread(m_workerThread);
    m_mpv = m_mpvController->mpv();

    // clang-format off
    connect(this, &MpvAbstractItem::setMpvProperty,
            m_mpvController, &MpvController::setProperty, Qt::QueuedConnection);

    connect(this, &MpvAbstractItem::mpvCommand,
            m_mpvController, &MpvController::command, Qt::QueuedConnection);
    // clang-format on
}

MpvAbstractItem::~MpvAbstractItem()
{
    if (m_mpv_gl) {
        mpv_render_context_free(m_mpv_gl);
    }
    mpv_set_wakeup_callback(m_mpv, nullptr, nullptr);
    mpv_terminate_destroy(m_mpv);

    m_workerThread->quit();
    m_workerThread->wait();
    m_workerThread->deleteLater();
    delete m_mpvController;
}

QQuickFramebufferObject::Renderer *MpvAbstractItem::createRenderer() const
{
    return new MpvRenderer(const_cast<MpvAbstractItem *>(this));
}

void MpvAbstractItem::setProperty(const QString &name, const QVariant &value)
{
    Q_EMIT setMpvProperty(name, value);
}

QVariant MpvAbstractItem::getProperty(const QString &name)
{
    return m_mpvController->getProperty(name);
}

void MpvAbstractItem::command(const QStringList &params)
{
    Q_EMIT mpvCommand(params);
}

#include "moc_mpvabstractitem.cpp"
