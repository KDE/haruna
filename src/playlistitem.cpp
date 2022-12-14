/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "playlistitem.h"

#include <QFileInfo>
#include <QUrl>

PlayListItem::PlayListItem(const QString &path, QObject *parent)
    : QObject(parent)
{
    QUrl url(path);

    if (url.scheme().startsWith("http")) {
        setFilePath(url.toString());
        setFileName(QString());
        setFolderPath(QString());
        setMediaTitle(url.toString());
    } else {
        QFileInfo fileInfo(path);
        setFileName(fileInfo.fileName());
        setFilePath(fileInfo.absoluteFilePath());
        setFolderPath(fileInfo.absolutePath());
    }
}

QString PlayListItem::mediaTitle() const
{
    return m_mediaTitle;
}

void PlayListItem::setMediaTitle(const QString &title)
{
    m_mediaTitle = title;
}

QString PlayListItem::filePath() const
{
    return m_filePath;
}

void PlayListItem::setFilePath(const QString &filePath)
{
    m_filePath = filePath;
}

QString PlayListItem::fileName() const
{
    return m_fileName;
}

void PlayListItem::setFileName(const QString &fileName)
{
    m_fileName = fileName;
}

QString PlayListItem::folderPath() const
{
    return m_folderPath;
}

void PlayListItem::setFolderPath(const QString &folderPath)
{
    m_folderPath = folderPath;
}

QString PlayListItem::duration() const
{
    return m_duration;
}

void PlayListItem::setDuration(const QString &duration)
{
    m_duration = duration;
}
