/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "track.h"

Track::Track(QObject *parent)
    : QObject(parent)
{
}

QString Track::lang() const
{
    return m_lang;
}

void Track::setLang(const QString &lang)
{
    m_lang = lang;
}

QString Track::title() const
{
    return m_title;
}

void Track::setTitle(const QString &title)
{
    m_title = title;
}

QString Track::codec() const
{
    return m_codec;
}

void Track::setCodec(const QString &codec)
{
    m_codec = codec;
}

qlonglong Track::id() const
{
    return m_id;
}

void Track::setId(const qlonglong &id)
{
    m_id = id;
}

qlonglong Track::ffIndex() const
{
    return m_ffIndex;
}

void Track::setFfIndex(const qlonglong &ffIndex)
{
    m_ffIndex = ffIndex;
}

qlonglong Track::srcId() const
{
    return m_srcId;
}

void Track::setSrcId(const qlonglong &srcId)
{
    m_srcId = srcId;
}

bool Track::dependent() const
{
    return m_dependent;
}

void Track::setDependent(bool dependent)
{
    m_dependent = dependent;
}

bool Track::external() const
{
    return m_external;
}

void Track::setExternal(bool external)
{
    m_external = external;
}

bool Track::forced() const
{
    return m_forced;
}

void Track::setForced(bool forced)
{
    m_forced = forced;
}

bool Track::defaut() const
{
    return m_defaut;
}

void Track::setDefaut(bool defaut)
{
    m_defaut = defaut;
}

QString Track::type() const
{
    return m_type;
}

void Track::setType(const QString &type)
{
    m_type = type;
}

int Track::index() const
{
    return m_index;
}

void Track::setIndex(int index)
{
    m_index = index;
}

QString Track::text()
{
    QString text;
    if (!m_title.isEmpty()) {
        text += m_title + " ";
    }
    if (!m_lang.isEmpty()) {
        text += m_lang + " ";
    }
    if (!m_codec.isEmpty()) {
        text += m_codec;
    }
    return text;
}

#include "moc_track.cpp"
