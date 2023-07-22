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
    connect(m_workerThread, &QThread::finished,
            m_mpvController, &MpvController::deleteLater);

    connect(this, QOverload<const QString &, const QVariant &>::of(&MpvAbstractItem::setMpvProperty),
            m_mpvController, QOverload<const QString &, const QVariant &>::of(&MpvController::setProperty),
            Qt::QueuedConnection);

    connect(this, QOverload<MpvController::Properties, const QVariant &>::of(&MpvAbstractItem::setMpvProperty),
            m_mpvController, QOverload<MpvController::Properties, const QVariant &>::of(&MpvController::setProperty),
            Qt::QueuedConnection);

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

    m_workerThread->quit();
    m_workerThread->wait();
    m_workerThread->deleteLater();
    mpv_terminate_destroy(m_mpv);
}

QQuickFramebufferObject::Renderer *MpvAbstractItem::createRenderer() const
{
    return new MpvRenderer(const_cast<MpvAbstractItem *>(this));
}

void MpvAbstractItem::setProperty(const QString &name, const QVariant &value)
{
    Q_EMIT setMpvProperty(name, value);
}

void MpvAbstractItem::setProperty(MpvController::Properties property, const QVariant &value)
{
    auto name = m_mpvController->properties().value(property);
    setProperty(name, value);
}

QVariant MpvAbstractItem::getProperty(const QString &name)
{
    return m_mpvController->getProperty(name);
}

QVariant MpvAbstractItem::getProperty(MpvController::Properties property)
{
    auto name = m_mpvController->properties().value(property);
    return getProperty(name);
}

void MpvAbstractItem::cachePropertyValue(const QString &property, const QVariant &value)
{
    m_propertiesCache[property] = value;
}

QVariant MpvAbstractItem::getCachedPropertyValue(const QString &property)
{
    if (!m_propertiesCache[property].isValid()) {
        auto value = getProperty(property);
        cachePropertyValue(property, value);
        return value;
    }
    return m_propertiesCache[property];
}

QVariant MpvAbstractItem::expandText(const QString &text)
{
    return m_mpvController->command(QStringList{QStringLiteral("expand-text"), text});
}

void MpvAbstractItem::command(const QStringList &params)
{
    Q_EMIT mpvCommand(params);
}

QVariant MpvAbstractItem::synchronousCommand(const QStringList &params)
{
    return m_mpvController->command(params);
}

#include "moc_mpvabstractitem.cpp"
