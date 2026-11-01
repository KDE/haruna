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

    remapCurrentRow();
    Q_EMIT searchTextChanged();
}

int TranscriptProxyModel::currentRow()
{
    return m_currentRow;
}

QString TranscriptProxyModel::filterString()
{
    return m_filterString;
}

void TranscriptProxyModel::setFilterString(const QString &filter)
{
    beginFilterChange();
    m_filterString = filter;
    endFilterChange();

    remapCurrentRow();
    Q_EMIT filterStringChanged();
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
    connect(m_transcriptModel, &TranscriptModel::parsingFinished, this, &TranscriptProxyModel::remapCurrentRow, Qt::UniqueConnection);

    Q_EMIT transcriptModelChanged();
}

bool TranscriptProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if (!m_transcriptModel) {
        return false;
    }

    auto style = m_transcriptModel->data(m_transcriptModel->index(source_row, 0), TranscriptModel::Roles::StyleRole).toString();

    bool searchMatch = QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
    bool styleMatch = m_filterString == u""_s || m_filterString == style;

    return searchMatch && styleMatch;
}

void TranscriptProxyModel::onCurrentRowChanged()
{
    auto sourceRow = m_transcriptModel->currentRow();
    if (sourceRow == -1) {
        m_currentRow = -1;
    } else {
        m_currentRow = findNextRow(sourceRow);
    }

    Q_EMIT currentRowChanged();
}

void TranscriptProxyModel::remapCurrentRow()
{
    if (!m_transcriptModel || rowCount() < 0) {
        m_currentRow = 0;
    } else {
        auto sourceRow = m_transcriptModel->currentRow();
        m_currentRow = findNextRow(sourceRow);
    }

    Q_EMIT currentRowChanged();
}

int TranscriptProxyModel::findNextRow(int sourceRow)
{
    auto row = mapFromSource(m_transcriptModel->index(sourceRow, 0)).row();
    if (row == -1) {
        // If sourceRow is hidden in the proxy, we should jump the view to the upcoming subtitle.
        // Check all proxy rows with another binary search
        std::vector<int> proxyRows(rowCount());
        std::iota(proxyRows.begin(), proxyRows.end(), 0);

        auto it = std::lower_bound(proxyRows.begin(), proxyRows.end(), sourceRow, [this](const int &row, const int &sourceRow) {
            return mapToSource(index(row, 0)).row() < sourceRow;
        });

        if (it != proxyRows.end()) {
            row = *it;
        }
    }
    return row;
}

#include "moc_transcriptproxymodel.cpp"
