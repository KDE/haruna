/*
 * SPDX-FileCopyrightText: 2026 Muhammet Sadık Uğursoy <sadikugursoy@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef TRANSCRIPTMODEL_H
#define TRANSCRIPTMODEL_H

#include <QAbstractListModel>
#include <QThreadPool>
#include <QtQml/qqmlregistration.h>

struct SubtitleLine;
class SubtitleParser;

struct EndTime {
    int index;
    double endTime;
};

class TranscriptModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit TranscriptModel(QObject *parent = nullptr);
    ~TranscriptModel();

    enum Roles {
        TextRole = Qt::UserRole,
        SplittedTextRole,
        DurationRole,
        StartTimeRole,
        FormattedStartTimeRole,
        FormattedEndTimeRole,
        CurrentRole,
    };
    Q_ENUM(Roles)

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_PROPERTY(int currentIndex READ currentIndex NOTIFY currentIndexChanged)
    int currentIndex();

    Q_INVOKABLE void loadSubtitle(QUrl filePath, int streamIndex, double position);
    Q_INVOKABLE void clearSubtitle();
    Q_INVOKABLE void updateCurrentEndTimes(double time);

Q_SIGNALS:
    void currentIndexChanged();
    void parsingFinished(int index);

private:
    void addItem(const SubtitleLine &item, const int transcriptModelVersion);
    void appendEndTime(int index, double endTime);
    void prependEndTime(int index, double endTime);
    void removeEndTime(int index);
    void clearEndTimes();
    void binarySearchEndTimes(double time);

    // Indexes for currently displayed subtitle lines in the m_transcript.
    QList<EndTime> m_currentEndTimes;
    int m_lastIndex{0};
    double m_lastTimePosition{-1.0};
    // Index for subtitle stream in the list of loaded subtitles
    int m_streamIndex{-1};
    bool m_parsingFinished{false};
    QList<SubtitleLine> m_transcript;
    std::unique_ptr<SubtitleParser> m_parser;
    QThreadPool m_threadPool;
    // incremented when parser is cancelled, SubtitleLine items with mismatching version are ignored inside addItem
    std::atomic<int> m_transcriptModelVersion{0};
    // abort worker threads if this value is true
    std::atomic<bool> m_cancelRequested{false};
};

#endif // TRANSCRIPTMODEL_H
