/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "tracksmodel.h"

using namespace Qt::StringLiterals;

TracksModel::TracksModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int TracksModel::rowCount(const QModelIndex & /*parent*/) const
{
    return m_tracks.size();
}

QVariant TracksModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || m_tracks.isEmpty()) {
        return QVariant();
    }

    QMap<QString, QVariant> track = m_tracks[index.row()].toMap();

    switch (role) {
    case TextRole: {
        QString text;
        auto title = track[u"title"_s].toString();
        if (!title.isEmpty()) {
            text += title.append(u" "_s);
        }
        auto lang = track[u"lang"_s].toString();
        if (!lang.isEmpty()) {
            text += lang.append(u" "_s);
        }
        auto codec = track[u"codec"_s].toString();
        if (!codec.isEmpty()) {
            text += codec;
        }
        return text;
    }
    case LanguageRole:
        return track[u"lang"_s];
    case TitleRole:
        return track[u"title"_s];
    case IdRole:
        return track[u"id"_s];
    case CodecRole:
        return track[u"codec"_s];
    }

    return QVariant();
}

QHash<int, QByteArray> TracksModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TextRole] = "displayText";
    roles[LanguageRole] = "language";
    roles[TitleRole] = "title";
    roles[IdRole] = "trackId";
    roles[CodecRole] = "codec";
    return roles;
}

void TracksModel::setTracks(QList<QVariant> tracks)
{
    beginResetModel();
    m_tracks = tracks;
    endResetModel();
}

#include "moc_tracksmodel.cpp"
