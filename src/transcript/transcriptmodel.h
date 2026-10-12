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
    int row;
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
        StyleRole,
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

    Q_PROPERTY(QStringList styles READ styles WRITE setStyles NOTIFY stylesChanged FINAL)
    Q_SIGNAL void stylesChanged();
    QStringList styles() const;
    void setStyles(const QStringList &newStyles);

    int currentRow() const;
    bool isCurrentEndTime(int row) const;
    Q_INVOKABLE void loadSubtitle(QUrl filePath, int streamIndex, double position);
    Q_INVOKABLE void clearSubtitle();
    Q_INVOKABLE void updateCurrentEndTimes(double time);
    Q_INVOKABLE void cancelParsing();

Q_SIGNALS:
    void currentRowChanged();
    void parsingFinished(int row);

private:
    void addItem(const SubtitleLine &item, const int transcriptModelVersion);
    void appendEndTime(int row, double endTime);
    void prependEndTime(int row, double endTime);
    void removeEndTime(int endTimeIndex);
    void clearEndTimes();
    void binarySearchEndTimes(double time);

    // Row numbers for currently displayed subtitle lines in the m_transcript.
    QList<EndTime> m_currentEndTimes;
    int m_lastRow{0};
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
    QStringList m_styles;
};

#endif // TRANSCRIPTMODEL_H
