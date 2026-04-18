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

int TracksModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return m_data.size();
}

QVariant TracksModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || m_data.isEmpty()) {
        return QVariant();
    }

    const auto item = m_data.at(index.row());

    switch (role) {
    case IdRole:
        return item.trackid;
    case TextRole:
        return item.display();
    case LanguageRole:
        return item.lang;
    case TitleRole:
        return item.title;
    case CodecRole:
        return item.codec;
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
    m_data.clear();
    endResetModel();
}

void TracksModel::addTrack(Track track)
{
    beginInsertRows(QModelIndex(), m_data.count(), m_data.count());
    m_data.append(track);
    endInsertRows();
    Q_EMIT rowCountChanged();
}

const Track TracksModel::track(int row) const
{
    return m_data.at(row);
}

void TracksModel::setTracks(QList<Track> tracks)
{
    beginResetModel();
    m_data = tracks;
    endResetModel();
    Q_EMIT rowCountChanged();
}

int TracksModel::nextRow()
{
    auto row = activeRow() + 1;
    if (row < 0 || row >= rowCount()) {
        return 0;
    }

    return row;
}

int TracksModel::previousRow()
{
    auto row = activeRow() - 1;
    if (row < 0 || row >= rowCount()) {
        return rowCount() - 1;
    }

    return row;
}

int TracksModel::activeRow() const
{
    return m_activeRow;
}

void TracksModel::setActiveRow(int newActiveTrack)
{
    m_activeRow = newActiveTrack;
}

#include "moc_tracksmodel.cpp"
