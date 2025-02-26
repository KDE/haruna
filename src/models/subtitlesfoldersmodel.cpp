/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "subtitlesfoldersmodel.h"

#include <KConfig>
#include <KConfigGroup>

#include "subtitlessettings.h"

SubtitlesFoldersModel::SubtitlesFoldersModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_list(SubtitlesSettings::subtitlesFolders())
{
}

int SubtitlesFoldersModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid()) {
        return 0;
    }

    return m_list.size();
}

QVariant SubtitlesFoldersModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    QString path = m_list[index.row()];

    switch (role) {
    case PathRole:
        return QVariant(path);
    }

    return QVariant();
}

QHash<int, QByteArray> SubtitlesFoldersModel::roleNames() const
{
    return {{Roles::PathRole, QByteArray("path")}};
}

void SubtitlesFoldersModel::updateFolder(const QString &folder, int row)
{
    m_list.replace(row, folder);
    QStringList newList = m_list;
    // remove empty strings
    // removing directly from m_list messes with the ui logic
    newList.removeAll(QString());

    SubtitlesSettings::self()->setSubtitlesFolders(newList);
    SubtitlesSettings::self()->save();

    Q_EMIT dataChanged(index(row, 0), index(row, 0));
}

void SubtitlesFoldersModel::deleteFolder(int row)
{
    beginRemoveRows(QModelIndex(), row, row);
    m_list.removeAt(row);
    endRemoveRows();

    SubtitlesSettings::setSubtitlesFolders(m_list);
    SubtitlesSettings::self()->save();
}

void SubtitlesFoldersModel::addFolder()
{
    beginInsertRows(QModelIndex(), m_list.size(), m_list.size());
    m_list.append(QString());
    endInsertRows();
}

#include "moc_subtitlesfoldersmodel.cpp"
