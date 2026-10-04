/*
 * SPDX-FileCopyrightText: 2026 Muhammet Sadık Uğursoy <sadikugursoy@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "transcriptmodel.h"

#include <QTimer>
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
    case SplittedTextRole:
        return item.text.split(u"\n"_s);
    case DurationRole:
        return item.duration;
    case StartTimeRole:
        return item.startTime;
    case FormattedStartTimeRole:
        return item.formattedStartTime;
    case FormattedEndTimeRole:
        return item.formattedEndTime;
    case CurrentRole:
        for (auto &endTime : m_currentEndTimes) {
            if (endTime.index == index.row()) {
                return true;
            }
        }
        return false;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> TranscriptModel::roleNames() const
{
    // clang-format off
    QHash<int, QByteArray> roles = {
    {TextRole,               QByteArrayLiteral("subtitleText")},
    {SplittedTextRole,       QByteArrayLiteral("splittedText")},
    {DurationRole,           QByteArrayLiteral("duration")},
    {StartTimeRole,          QByteArrayLiteral("startTime")},
    {FormattedStartTimeRole, QByteArrayLiteral("formattedStartTime")},
    {FormattedEndTimeRole,   QByteArrayLiteral("formattedEndTime")},
    {CurrentRole,            QByteArrayLiteral("isCurrent")},
    };
    // clang-format on

    return roles;
}

int TranscriptModel::currentIndex()
{
    if (m_currentEndTimes.empty()) {
        return -1;
    }
    // Return the smallest index. This value is used by a ListView as its currentItem and we are aligning the items to top.
    // When multiple items are highlighted, the smallest index should be aligned to top, assuming the list is sorted.
    return m_currentEndTimes[0].index;
}

void TranscriptModel::loadSubtitle(QUrl filePath, int streamIndex, double position)
{
    clearSubtitle();
    m_streamIndex = streamIndex;
    m_cancelRequested = true;

    m_threadPool.clear();
    m_threadPool.waitForDone();

    m_cancelRequested = false;
    m_parsingFinished = false;

    const auto expectedTranscriptModelVersion = m_transcriptModelVersion.load();
    m_threadPool.start([this, filePath, streamIndex, position, expectedTranscriptModelVersion]() {
        m_parser->parseSubtitle(filePath, streamIndex, expectedTranscriptModelVersion, m_cancelRequested);

        if (m_cancelRequested || expectedTranscriptModelVersion != m_transcriptModelVersion) {
            return;
        }

        // highlighted index can be an index that is not yet added to m_transcript
        // therefore queued signals from parseSubtitle should be waited before checking highlights.
        QTimer::singleShot(0, this, [this, position]() {
            m_lastTimePosition = -1.0;
            m_lastIndex = 0;
            m_parsingFinished = true;

            binarySearchEndTimes(position);

            if (m_currentEndTimes.empty()) {
                // If no subtitle is shown, scroll to the current position
                Q_EMIT parsingFinished(m_lastIndex);
            } else {
                Q_EMIT parsingFinished(currentIndex());
            }
        });
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
    clearEndTimes();
}

// When mpv position changes, update current indexes by checking pTime. Any new index should satisfy the condition: {startTime < pTime < endTime}.
// Note that multiple subtitles can satisfy the above condition at any pTime. So we should store them in a list.
// SubtitleLine array is sorted by startTime, this allows binary search for faster results. We can also assume the media will be played normaly
// without seeking back and forth most of the time. So we can make incremental checks from the highlighted indexes for saving some performance.
void TranscriptModel::updateCurrentEndTimes(double pTime)
{
    if (!m_parsingFinished) {
        return;
    }

    bool backwardSeek = false;
    bool forwardSeek = false;

    // Compare pTime with m_lastTimePosition to detect seeks. For forward seeks, if it is not a big jump, do not remake the search.
    if (m_lastTimePosition > 0.0) {
        backwardSeek = pTime < m_lastTimePosition;
        forwardSeek = pTime - m_lastTimePosition > 10.0;
    }

    if (m_lastTimePosition < 0.0 || backwardSeek || forwardSeek) {
        binarySearchEndTimes(pTime);
        return;
    }

    auto time = pTime * 1000.0;
    // No seeks are made, check incrementally from the last position
    // Remove subtitles that are no longed displayed
    for (int i = 0; i < m_currentEndTimes.size(); ++i) {
        auto &currentEndTime = m_currentEndTimes[i];
        if (currentEndTime.endTime < time) {
            removeEndTime(i);
        }
    }

    while (m_lastIndex < m_transcript.size() && m_transcript[m_lastIndex].startTime <= time) {
        auto endTime = m_transcript[m_lastIndex].startTime + m_transcript[m_lastIndex].duration;
        if (endTime >= time) {
            appendEndTime(m_lastIndex, endTime);
        }
        m_lastIndex++;
    }
    m_lastTimePosition = pTime;
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

void TranscriptModel::appendEndTime(int pIndex, double endTime)
{
    for (auto &currentEndTime : m_currentEndTimes) {
        if (currentEndTime.index == pIndex) {
            return;
        }
    }

    m_currentEndTimes.append(EndTime(pIndex, endTime));

    Q_EMIT dataChanged(index(pIndex, 0), index(pIndex, 0));
    Q_EMIT currentIndexChanged();
}

void TranscriptModel::prependEndTime(int pIndex, double endTime)
{
    for (auto &currentEndTime : m_currentEndTimes) {
        if (currentEndTime.index == pIndex) {
            return;
        }
    }

    m_currentEndTimes.prepend(EndTime(pIndex, endTime));

    Q_EMIT dataChanged(index(pIndex, 0), index(pIndex, 0));
    Q_EMIT currentIndexChanged();
}

void TranscriptModel::removeEndTime(int pEndTimeIndex)
{
    auto removedIndex = m_currentEndTimes[pEndTimeIndex].index;
    m_currentEndTimes.remove(pEndTimeIndex);
    Q_EMIT dataChanged(index(removedIndex, 0), index(removedIndex, 0));
    Q_EMIT currentIndexChanged();
}

void TranscriptModel::clearEndTimes()
{
    QList<int> indexes;
    for (int i = 0; i < m_currentEndTimes.size(); ++i) {
        indexes.append(m_currentEndTimes[i].index);
    }

    m_currentEndTimes.clear();

    for (auto &removedIndex : indexes) {
        Q_EMIT dataChanged(index(removedIndex, 0), index(removedIndex, 0));
    }
    Q_EMIT currentIndexChanged();
}

// Finds all EndTimes containing pTime where {startTime < pTime < endTime}, and adds them to m_currentEndTimes
void TranscriptModel::binarySearchEndTimes(double pTime)
{
    clearEndTimes();

    auto it = std::lower_bound(m_transcript.begin(), m_transcript.end(), pTime * 1000.0, [](const SubtitleLine &line, const double &time) {
        return line.startTime < time;
    });

    // Lower bound actually returns the upcomping subtitle. Just get the previous item.
    it = std::prev(it);
    m_lastIndex = std::distance(m_transcript.begin(), it);
    if (m_lastIndex < 0) {
        m_lastIndex = -1;
        m_lastTimePosition = -1;
        return;
    }

    // We can iterate backward from that point and check subtitles that falls within the EndTime.
    int prevIndex = m_lastIndex;
    int searchDepth = std::min(prevIndex, 5);
    while (prevIndex > 0) {
        auto &startTime = m_transcript[prevIndex].startTime;
        auto endTime = startTime + m_transcript[prevIndex].duration;
        // Don't exit the loop immediately after finding an expired subtitle.
        // Continue scanning until 5 consecutive expired subtitles have been found.
        // An expired subtitle does not imply that earlier subtitles are also expired,
        // since subtitles are sorted by startTime, not endTime.
        // An expired subtitle is one whose endTime is less than pTime.
        // 5 is an arbitrary value.
        if (endTime >= pTime * 1000.0) {
            prependEndTime(prevIndex, endTime);
            prevIndex--;
            searchDepth = std::min(prevIndex, 5);
            continue;
        }
        searchDepth--;
        prevIndex--;
        if (searchDepth < 0) {
            break;
        }
    }
    m_lastTimePosition = pTime;
}

#include "moc_transcriptmodel.cpp"
