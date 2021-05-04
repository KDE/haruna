/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "_debug.h"
#include "track.h"
#include "tracksmodel.h"
#include <utility>

TracksModel::TracksModel(QObject *parent)
    : QAbstractListModel(parent)
{

}

int TracksModel::rowCount(const QModelIndex &/*parent*/) const
{
    return m_tracks.size();
}

QVariant TracksModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || m_tracks.isEmpty())
        return QVariant();

    Track *track = m_tracks[index.row()];

    switch (role) {
    case TextRole:
        return QVariant(track->text());
    case LanguageRole:
        return QVariant(track->lang());
    case TitleRole:
        return QVariant(track->title());
    case IDRole:
        return QVariant(track->id());
    case CodecRole:
        return QVariant(track->codec());
    }

    return QVariant();
}

QHash<int, QByteArray> TracksModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TextRole] = "text";
    roles[LanguageRole] = "language";
    roles[TitleRole] = "title";
    roles[IDRole] = "id";
    roles[CodecRole] = "codec";
    return roles;
}

void TracksModel::setTracks(QMap<int, Track *> tracks)
{
    beginResetModel();
    m_tracks = std::move(tracks);
    endResetModel();
}
