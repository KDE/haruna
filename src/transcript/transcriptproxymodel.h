/*
 * SPDX-FileCopyrightText: 2026 Muhammet Sadık Uğursoy <sadikugursoy@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef TRANSCRIPTPROXYMODEL_H
#define TRANSCRIPTPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QtQml/qqmlregistration.h>

class TranscriptModel;
class TranscriptProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit TranscriptProxyModel(QObject *parent = nullptr);

    Q_PROPERTY(QString searchText READ searchText WRITE setSearchText NOTIFY searchTextChanged)
    QString searchText();
    void setSearchText(QString text);

    Q_PROPERTY(int currentRow READ currentRow NOTIFY currentRowChanged)
    int currentRow();

    Q_PROPERTY(TranscriptModel *transcriptModel READ transcriptModel WRITE setTranscriptModel NOTIFY transcriptModelChanged FINAL REQUIRED)
    TranscriptModel *transcriptModel();
    void setTranscriptModel(TranscriptModel *model);

Q_SIGNALS:
    void searchTextChanged();
    void currentRowChanged();
    void transcriptModelChanged();
    void parsingFinished(int row);

private:
    int mapCurrentRow(int row);
    void onParsingFinished(int row);

    TranscriptModel *m_transcriptModel{nullptr};
    QString m_searchText;
};

#endif // TRANSCRIPTPROXYMODEL_H
