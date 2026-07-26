/*
 * SPDX-FileCopyrightText: 2026 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef MPRISITEM_H
#define MPRISITEM_H

#include <QObject>
#include <qqmlintegration.h>

#include "config-haruna.h"

class MprisItemPrivate;
class MpvItem;
class PlaylistFilterProxyModel;
class QWindow;

class MprisItem : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit MprisItem(QObject *parent = nullptr);
    ~MprisItem();

    Q_PROPERTY(MpvItem *mpv READ mpv WRITE setMpv NOTIFY mpvChanged FINAL REQUIRED)
    Q_SIGNAL void mpvChanged();
    MpvItem *mpv() const;
    void setMpv(MpvItem *newMpv);

    Q_PROPERTY(PlaylistFilterProxyModel *activePlaylist READ activePlaylist WRITE setActivePlaylist NOTIFY activePlaylistChanged FINAL)
    Q_SIGNAL void activePlaylistChanged();
    PlaylistFilterProxyModel *activePlaylist() const;
    void setActivePlaylist(PlaylistFilterProxyModel *newActivePlaylist);

    Q_PROPERTY(QWindow *window READ window WRITE setWindow NOTIFY windowChanged FINAL REQUIRED)
    Q_SIGNAL void windowChanged();
    QWindow *window() const;
    void setWindow(QWindow *newWindow);

    Q_SIGNAL void raise();
    Q_SIGNAL void openUri();

#if HAVE_DBUS
private:
    void setupMpvConnections();
    void getAudioThumbnail(const QUrl &url);
    QString findAudioThumbnail(const QString &filePath);
    void setAudioThumbnail(const QString &image);
    QByteArray embeddedArtwork(const QUrl &url);
    std::unique_ptr<MprisItemPrivate> d_ptr;
#endif
};

#endif // MPRISITEM_H
