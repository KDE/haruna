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

class TranscriptModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit TranscriptModel(QObject *parent = nullptr);
    ~TranscriptModel();

    enum Roles {
        TextRole = Qt::UserRole,
        DurationRole,
        StartTimeRole,
        EndTimeRole,
        FormattedStartTimeRole,
        FormattedEndTimeRole,
        CurrentRole,
    };
    Q_ENUM(Roles)

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    int currentIndex();

    Q_INVOKABLE void loadSubtitle(QUrl filePath, int streamIndex);
    Q_INVOKABLE void clearSubtitle();

Q_SIGNALS:
    void currentIndexChanged();

private:
    void addItem(const SubtitleLine &item, const int transcriptModelVersion);
    void setCurrentIndex(int index);

    // Index for currently displayed subtitle line in the m_transcript. -1 if nothing is displayed at the current timeframe.
    int m_currentIndex{-1};
    // Index for subtitle stream in the list of loaded subtitles
    int m_streamIndex{-1};
    QList<SubtitleLine> m_transcript;
    std::unique_ptr<SubtitleParser> m_parser;
    QThreadPool m_threadPool;
    // incremented when parser is cancelled, SubtitleLine items with mismatching version are ignored inside addItem
    std::atomic<int> m_transcriptModelVersion{0};
    // abort worker threads if this value is true
    std::atomic<bool> m_cancelRequested{false};
};

#endif // TRANSCRIPTMODEL_H
