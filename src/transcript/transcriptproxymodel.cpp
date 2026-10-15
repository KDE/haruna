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
    Q_EMIT currentIndexChanged();
}

QPersistentModelIndex TranscriptProxyModel::currentIndex()
{
    return m_currentIndex;
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

    connect(m_transcriptModel, &TranscriptModel::currentRowChanged, this, &TranscriptProxyModel::onCurrentRowChanged, Qt::UniqueConnection);
    connect(m_transcriptModel, &TranscriptModel::parsingFinished, this, &TranscriptProxyModel::onParsingFinished, Qt::UniqueConnection);

    Q_EMIT transcriptModelChanged();
}

void TranscriptProxyModel::onParsingFinished(int sourceRow)
{
    if (!m_transcriptModel || rowCount() < 0) {
        Q_EMIT parsingFinished(QPersistentModelIndex(index(0, 0)));
        return;
    }

    auto modelIndex = mapFromSource(m_transcriptModel->index(sourceRow, 0));
    if (!modelIndex.isValid()) {
        // If sourceRow is hidden in the proxy, we should jump the view to the upcoming subtitle.
        // Check all proxy rows with another binary search
        std::vector<int> proxyRows(rowCount());
        std::iota(proxyRows.begin(), proxyRows.end(), 0);

        auto it = std::lower_bound(proxyRows.begin(), proxyRows.end(), modelIndex, [this](const int &row, const QModelIndex &modelIndex) {
            return index(row, 0) < modelIndex;
        });

        if (it != proxyRows.end()) {
            modelIndex = index(*it, 0);
        }
    }

    Q_EMIT parsingFinished(QPersistentModelIndex(modelIndex));
}

void TranscriptProxyModel::onCurrentRowChanged()
{
    if (!m_transcriptModel) {
        return;
    }

    auto sourceModelIndex = m_transcriptModel->index(m_transcriptModel->currentRow(), 0);
    m_currentIndex = QPersistentModelIndex(mapFromSource(sourceModelIndex));

    Q_EMIT currentIndexChanged();
}

#include "moc_transcriptproxymodel.cpp"
