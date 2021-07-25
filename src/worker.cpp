/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "worker.h"

#include "application.h"

#include <KFileItem>
#include <QThread>
#include <KFileMetaData/ExtractorCollection>
#include <KFileMetaData/SimpleExtractionResult>

Worker* Worker::instance()
{
    static Worker w;
    return &w;
}

void Worker::getMetaData(int index, const QString &path)
{
    QString mimeType = Application::mimeType(path);
    KFileMetaData::ExtractorCollection exCol;
    QList<KFileMetaData::Extractor*> extractors = exCol.fetchExtractors(mimeType);
    KFileMetaData::SimpleExtractionResult result(path, mimeType,
                                                 KFileMetaData::ExtractionResult::ExtractMetaData);
    if (extractors.size() == 0) {
        return;
    }
    KFileMetaData::Extractor* ex = extractors.first();
    ex->extract(&result);
    auto properties = result.properties();

    emit metaDataReady(index, properties);
}
