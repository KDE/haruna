/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "_debug.h"
#include "subtitlesfoldersmodel.h"
#include "subtitlessettings.h"

#include <KConfig>
#include <KConfigDialog>
#include <KConfigGroup>

SubtitlesFoldersModel::SubtitlesFoldersModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_config = KSharedConfig::openConfig("georgefb/haruna.conf");
    m_list = m_config->group("Subtitles").readPathEntry("Folders", QStringList());
}

int SubtitlesFoldersModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return m_list.size();
}

QVariant SubtitlesFoldersModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();


    QString path = m_list[index.row()];

    switch (role) {
    case Qt::DisplayRole:
        return QVariant(path);
    }

    return QVariant();
}

void SubtitlesFoldersModel::updateFolder(const QString &folder, int row)
{
    m_list.replace(row, folder);
    QStringList newList = m_list;
    // remove empty strings
    // removing directly from m_list messes with the ui logic
    newList.removeAll(QString(""));

    SubtitlesSettings::self()->setSubtitlesFolders(newList);
    SubtitlesSettings::self()->save();
}

void SubtitlesFoldersModel::deleteFolder(int row)
{
    beginRemoveRows(QModelIndex(), row, row);
    m_list.removeAt(row);
    endRemoveRows();
    m_config->group("Subtitles").writePathEntry("Folders", m_list);
    m_config->sync();
}

void SubtitlesFoldersModel::addFolder()
{
    beginInsertRows(QModelIndex(), m_list.size(), m_list.size());
    m_list.append(QStringLiteral());
    endInsertRows();
}
