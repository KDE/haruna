/*
 * SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "chaptersmodel.h"

ChaptersModel::ChaptersModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int ChaptersModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return m_chapters.count();
}

QVariant ChaptersModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    auto chapter = m_chapters.at(index.row());

    switch (role) {
    case TitleRole:
        return QVariant(chapter.title);

    case StartTimeRole:
        return QVariant(chapter.startTime);
    }

    return QVariant();
}

QHash<int, QByteArray> ChaptersModel::roleNames() const
{
    // clang-format off
    QHash<int, QByteArray> roles{
        {TitleRole,     QByteArrayLiteral("title")},
        {StartTimeRole, QByteArrayLiteral("startTime")},
    };
    // clang-format on

    return roles;
}

void ChaptersModel::setChapters(QList<Chapter> &_chapters)
{
    beginResetModel();
    m_chapters = _chapters;
    endResetModel();
    Q_EMIT rowCountChanged();
}

#include "moc_chaptersmodel.cpp"
