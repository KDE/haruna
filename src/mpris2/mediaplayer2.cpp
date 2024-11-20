/*
 * SPDX-FileCopyrightText: 2021 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "mediaplayer2.h"

#include <KAboutData>

#include <QApplication>

using namespace Qt::StringLiterals;

MediaPlayer2::MediaPlayer2(QObject *obj)
    : QDBusAbstractAdaptor(obj)
{
}

void MediaPlayer2::Raise()
{
    Q_EMIT raise();
}

void MediaPlayer2::Quit()
{
    qApp->quit();
}

bool MediaPlayer2::CanRaise() const
{
    return true;
}

bool MediaPlayer2::CanQuit() const
{
    return true;
}

bool MediaPlayer2::HasTrackList() const
{
    return false;
}

QString MediaPlayer2::Identity() const
{
    return u"Haruna"_s;
}

QString MediaPlayer2::DesktopEntry() const
{
    return KAboutData::applicationData().desktopFileName();
}

QStringList MediaPlayer2::SupportedUriSchemes() const
{
    return QStringList() << u"file"_s << u"http"_s << u"https"_s;
}

QStringList MediaPlayer2::SupportedMimeTypes() const
{
    return QStringList() << u"video/*"_s << u"audio/*"_s;
}

#include "moc_mediaplayer2.cpp"
