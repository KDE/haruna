/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef PLAYLISTITEM_H
#define PLAYLISTITEM_H

#include <QObject>

class PlayListItem : public QObject
{
    Q_OBJECT
public:
    explicit PlayListItem(const QString &path, QObject *parent = nullptr);

    Q_INVOKABLE QString mediaTitle() const;
    void setMediaTitle(const QString &title);

    Q_INVOKABLE QString filePath() const;
    void setFilePath(const QString &filePath);

    Q_INVOKABLE QString fileName() const;
    void setFileName(const QString &fileName);

    Q_INVOKABLE QString folderPath() const;
    void setFolderPath(const QString &folderPath);

    Q_INVOKABLE QString duration() const;
    void setDuration(const QString &duration);

    Q_INVOKABLE bool isPlaying() const;
    void setIsPlaying(bool isPlaying);

private:
    QString m_mediaTitle;
    QString m_filePath;
    QString m_fileName;
    QString m_folderPath;
    QString m_duration;
    bool m_isHovered {false};
    bool m_isPlaying {false};
};

#endif // PLAYLISTITEM_H
