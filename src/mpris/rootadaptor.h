/*
 * SPDX-FileCopyrightText: 2021 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef MEDIAPLAYER2_H
#define MEDIAPLAYER2_H

#include <QDBusAbstractAdaptor>

class MprisController;
class QDBusObjectPath;

// https://specifications.freedesktop.org/mpris/latest/Media_Player.html
class RootAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.mpris.MediaPlayer2")

public:
    explicit RootAdaptor(MprisController &parent);
    ~RootAdaptor() = default;

    Q_PROPERTY(bool CanQuit READ canQuit CONSTANT)
    bool canQuit() const;

    Q_PROPERTY(bool Fullscreen READ fullscreen WRITE setFullscreen CONSTANT)
    bool fullscreen() const;
    void setFullscreen(bool newFullscreen);

    Q_PROPERTY(bool CanSetFullscreen READ canSetFullscreen CONSTANT)
    bool canSetFullscreen() const;

    Q_PROPERTY(bool CanRaise READ canRaise CONSTANT)
    bool canRaise() const;

    Q_PROPERTY(bool HasTrackList READ hasTrackList CONSTANT)
    bool hasTrackList() const;

    Q_PROPERTY(QString Identity READ identity CONSTANT)
    QString identity() const;

    Q_PROPERTY(QString DesktopEntry READ desktopEntry CONSTANT)
    QString desktopEntry() const;

    Q_PROPERTY(QStringList SupportedUriSchemes READ supportedUriSchemes CONSTANT)
    QStringList supportedUriSchemes() const;

    Q_PROPERTY(QStringList SupportedMimeTypes READ supportedMimeTypes CONSTANT)
    QStringList supportedMimeTypes() const;

    void notify(const QString &property, const QVariant &value);

public Q_SLOTS:
    void Raise();
    void Quit();

private:
    MprisController &m_mprisController;
};

#endif // MEDIAPLAYER2_H
