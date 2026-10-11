/*
 * SPDX-FileCopyrightText: 2026 Muhammet Sadık Uğursoy <sadikugursoy@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "transcriptproxymodel.h"
#include "transcriptmodel.h"

using namespace Qt::StringLiterals;

TranscriptProxyModel::TranscriptProxyModel(QObject *parent)
    : QSortFilterProxyModel{parent}
{
    setDynamicSortFilter(true);
    setFilterCaseSensitivity(Qt::CaseInsensitive);
    setFilterRole(TranscriptModel::TextRole);
    invalidate();
}

QString TranscriptProxyModel::searchText()
{
    return m_searchText;
}

void TranscriptProxyModel::setSearchText(QString text = QString())
{
    beginFilterChange();
    m_searchText = text;
    setFilterRegularExpression(m_searchText);
    endFilterChange();

    Q_EMIT searchTextChanged();
    Q_EMIT currentRowChanged();
}

int TranscriptProxyModel::currentRow()
{
    if (!m_transcriptModel) {
        return -1;
    }
    return mapCurrentRow(m_transcriptModel->currentRow());
}

TranscriptModel *TranscriptProxyModel::transcriptModel()
{
    return m_transcriptModel;
}

void TranscriptProxyModel::setTranscriptModel(TranscriptModel *model)
{
    if (!model) {
        return;
    }

    m_transcriptModel = model;
    setSourceModel(m_transcriptModel);

    connect(m_transcriptModel, &TranscriptModel::currentRowChanged, this, &TranscriptProxyModel::currentRowChanged, Qt::UniqueConnection);
    connect(m_transcriptModel, &TranscriptModel::parsingFinished, this, &TranscriptProxyModel::onParsingFinished, Qt::UniqueConnection);

    Q_EMIT transcriptModelChanged();
}

int TranscriptProxyModel::mapCurrentRow(int row)
{
    auto modelIndex = m_transcriptModel->index(row, 0);
    if (!modelIndex.isValid()) {
        return -1;
    }

    auto proxyIndex = mapFromSource(modelIndex);
    if (!proxyIndex.isValid()) {
        return -1;
    }

    return proxyIndex.row();
}

void TranscriptProxyModel::onParsingFinished(int sourceRow)
{
    if (!m_transcriptModel || rowCount() < 0) {
        Q_EMIT parsingFinished(0);
        return;
    }

    int row = mapCurrentRow(sourceRow);

    if (row == -1) {
        // If sourceRow is hidden in the proxy, we should jump the view to the upcoming subtitle.
        // Check all proxy indexes with another binary search
        std::vector<int> proxyIndexes(rowCount());
        std::iota(proxyIndexes.begin(), proxyIndexes.end(), 0);

        auto it = std::lower_bound(proxyIndexes.begin(), proxyIndexes.end(), sourceRow, [this](const int &pIndex, const int &pSourceRow) {
            return mapToSource(index(pIndex, 0)).row() < pSourceRow;
        });

        if (it != proxyIndexes.end()) {
            row = *it;
        }
    }

    Q_EMIT parsingFinished(row);
}

#include "moc_transcriptproxymodel.cpp"
