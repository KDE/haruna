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

    Track track = m_tracks[index.row()];

    switch (role) {
    case IdRole:
        return track.trackid;
    case TextRole: {
        QString text;
        auto title = track.title;
        if (!title.isEmpty()) {
            text += title.append(u" "_s);
        }
        auto lang = track.lang;
        if (!lang.isEmpty()) {
            text += lang.append(u" "_s);
        }
        auto codec = track.codec;
        if (!codec.isEmpty()) {
            text += codec;
        }
        return text;
    }
    case LanguageRole:
        return track.lang;
    case TitleRole:
        return track.title;
    case CodecRole:
        return track.codec;
    }

    return QVariant();
}

QHash<int, QByteArray> TracksModel::roleNames() const
{
    // clang-format off
    QHash<int, QByteArray> roles{
        {IdRole,       QByteArrayLiteral("trackId")},
        {TextRole,     QByteArrayLiteral("displayText")},
        {LanguageRole, QByteArrayLiteral("language")},
        {TitleRole,    QByteArrayLiteral("title")},
        {CodecRole,    QByteArrayLiteral("codec")},
    };
    // clang-format on

    return roles;
}

void TracksModel::clear()
{
    beginResetModel();
    m_tracks.clear();
    endResetModel();
}

void TracksModel::addTrack(Track track)
{
    beginInsertRows(QModelIndex(), m_tracks.count(), m_tracks.count());
    m_tracks.append(track);
    endInsertRows();
    Q_EMIT rowCountChanged();
}

void TracksModel::setTracks(QList<Track> tracks)
{
    beginResetModel();
    m_tracks = tracks;
    endResetModel();
    Q_EMIT rowCountChanged();
}

#include "moc_tracksmodel.cpp"
