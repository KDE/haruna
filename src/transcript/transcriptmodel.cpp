/*
 * SPDX-FileCopyrightText: 2026 Muhammet Sadık Uğursoy <sadikugursoy@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "transcriptmodel.h"

#include <QUrl>

#include "subtitleline.h"
#include "subtitleparser.h"

using namespace Qt::StringLiterals;

TranscriptModel::TranscriptModel(QObject *parent)
    : QAbstractListModel{parent}
    , m_parser{std::make_unique<SubtitleParser>()}
{
    connect(m_parser.get(), &SubtitleParser::transcriptItemReady, this, &TranscriptModel::addItem, Qt::QueuedConnection);
}

TranscriptModel::~TranscriptModel()
{
    m_threadPool.clear();
    m_threadPool.waitForDone();
}

int TranscriptModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_transcript.size();
}

QVariant TranscriptModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    auto item = m_transcript.at(index.row());
    switch (role) {
    case TextRole:
        return item.text;
    case DurationRole:
        return item.duration;
    case StartTimeRole:
        return item.startTime;
    case EndTimeRole:
        return item.endTime;
    case FormattedStartTimeRole:
        return item.formattedStartTime;
    case FormattedEndTimeRole:
        return item.formattedEndTime;
    case CurrentRole:
        return false;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> TranscriptModel::roleNames() const
{
    // clang-format off
    QHash<int, QByteArray> roles = {
    {TextRole,               QByteArrayLiteral("text")},
    {DurationRole,           QByteArrayLiteral("duration")},
    {StartTimeRole,          QByteArrayLiteral("startTime")},
    {EndTimeRole,            QByteArrayLiteral("endTime")},
    {FormattedStartTimeRole, QByteArrayLiteral("formattedStartTime")},
    {FormattedEndTimeRole,   QByteArrayLiteral("formattedEndTime")},
    {CurrentRole,            QByteArrayLiteral("isCurrent")},
    };
    // clang-format on

    return roles;
}

void TranscriptModel::loadSubtitle(QUrl filePath, int streamIndex)
{
    clearSubtitle();
    m_streamIndex = streamIndex;
    m_cancelRequested = true;

    m_threadPool.clear();
    m_threadPool.waitForDone();

    m_cancelRequested = false;

    const auto expectedTranscriptModelVersion = m_transcriptModelVersion.load();
    m_threadPool.start([this, filePath, streamIndex, expectedTranscriptModelVersion]() {
        m_parser->parseSubtitle(filePath, streamIndex, expectedTranscriptModelVersion, m_cancelRequested);
    });
}

void TranscriptModel::clearSubtitle()
{
    m_transcriptModelVersion++;

    if (m_transcript.isEmpty()) {
        return;
    }

    beginResetModel();
    m_transcript.clear();
    endResetModel();
}

void TranscriptModel::addItem(const SubtitleLine &item, const int transcriptModelVersion)
{
    if (transcriptModelVersion != m_transcriptModelVersion) {
        return;
    }

    beginInsertRows(QModelIndex(), m_transcript.size(), m_transcript.size());
    m_transcript.push_back(item);
    endInsertRows();
}

int TranscriptModel::currentIndex()
{
    return m_currentIndex;
}

void TranscriptModel::setCurrentIndex(int index)
{
    if (m_currentIndex == index) {
        return;
    }

    m_currentIndex = index;

    Q_EMIT currentIndexChanged();
}

#include "moc_transcriptmodel.cpp"
