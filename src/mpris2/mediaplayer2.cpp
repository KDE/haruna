/*
 * SPDX-FileCopyrightText: 2021 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "mediaplayer2.h"

#include <KAboutData>

#include <QApplication>

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
    return QStringLiteral("Haruna");
}

QString MediaPlayer2::DesktopEntry() const
{
    return KAboutData::applicationData().desktopFileName();
}

QStringList MediaPlayer2::SupportedUriSchemes() const
{
    return QStringList() << QStringLiteral("file") << QStringLiteral("http") << QStringLiteral("https");
}

QStringList MediaPlayer2::SupportedMimeTypes() const
{
    return QStringList() << QStringLiteral("video/*") << QStringLiteral("audio/*");
}

#include "moc_mediaplayer2.cpp"
