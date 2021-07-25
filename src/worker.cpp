/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "worker.h"

#include <QMimeDatabase>
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
    QMimeDatabase db;
    QMimeType type = db.mimeTypeForFile(path);
    KFileMetaData::ExtractorCollection exCol;
    QList<KFileMetaData::Extractor*> extractors = exCol.fetchExtractors(type.name());
    KFileMetaData::SimpleExtractionResult result(path, type.name(),
                                                 KFileMetaData::ExtractionResult::ExtractMetaData);
    if (extractors.size() == 0) {
        return;
    }
    KFileMetaData::Extractor* ex = extractors.first();
    ex->extract(&result);
    auto properties = result.properties();

    emit metaDataReady(index, properties);
}
