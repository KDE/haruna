/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "tracksmodel.h"

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
        auto title = track[QStringLiteral("title")].toString();
        if (!title.isEmpty()) {
            text += title.append(QStringLiteral(" "));
        }
        auto lang = track[QStringLiteral("lang")].toString();
        if (!lang.isEmpty()) {
            text += lang.append(QStringLiteral(" "));
        }
        auto codec = track[QStringLiteral("codec")].toString();
        if (!codec.isEmpty()) {
            text += codec;
        }
        return text;
    }
    case LanguageRole:
        return track[QStringLiteral("lang")];
    case TitleRole:
        return track[QStringLiteral("title")];
    case IDRole:
        return track[QStringLiteral("id")];
    case CodecRole:
        return track[QStringLiteral("codec")];
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

void TracksModel::setTracks(QList<QVariant> tracks)
{
    beginResetModel();
    m_tracks = tracks;
    endResetModel();
}

#include "moc_tracksmodel.cpp"
