/*
 * SPDX-FileCopyrightText: 2022 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "mpvabstractitem.h"

#include <QThread>
#include <clocale>

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
}

MpvAbstractItem::~MpvAbstractItem()
{
    if (m_mpv_gl) {
        mpv_render_context_free(m_mpv_gl);
    }
    mpv_terminate_destroy(m_mpv);

    m_workerThread->quit();
    m_workerThread->deleteLater();
    delete m_mpvController;
}

QQuickFramebufferObject::Renderer *MpvAbstractItem::createRenderer() const
{
    return new MpvRenderer(const_cast<MpvAbstractItem *>(this));
}

int MpvAbstractItem::setProperty(const QString &name, const QVariant &value)
{
    return m_mpvController->setProperty(name, value);
}

QVariant MpvAbstractItem::getProperty(const QString &name)
{
    return m_mpvController->getProperty(name);
}

QVariant MpvAbstractItem::command(const QStringList &params)
{
    return m_mpvController->command(params);
}

#include "moc_mpvabstractitem.cpp"
