/*
 * SPDX-FileCopyrightText: 2025 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "miscutils.h"

#include <QCryptographicHash>

#include <KFileItem>
#include <KFileMetaData/ExtractorCollection>

using namespace Qt::StringLiterals;

MiscUtils *MiscUtils::instance()
{
    static MiscUtils i;
    return &i;
}

MiscUtils *MiscUtils::create(QQmlEngine *, QJSEngine *)
{
    QQmlEngine::setObjectOwnership(instance(), QQmlEngine::CppOwnership);
    return instance();
}

MiscUtils::MiscUtils()
    : QObject{nullptr}
{
}

QString MiscUtils::formatTime(const double time)
{
    int totalNumberOfSeconds = static_cast<int>(time);
    int seconds = totalNumberOfSeconds % 60;
    int minutes = (totalNumberOfSeconds / 60) % 60;
    int hours = (totalNumberOfSeconds / 60 / 60);

    QString hoursString = u"%1"_s.arg(hours, 2, 10, QLatin1Char('0'));
    QString minutesString = u"%1"_s.arg(minutes, 2, 10, QLatin1Char('0'));
    QString secondsString = u"%1"_s.arg(seconds, 2, 10, QLatin1Char('0'));
    QString timeString = u"%1:%2:%3"_s.arg(hoursString, minutesString, secondsString);

    return timeString;
}

QString MiscUtils::mimeType(const QUrl &url)
{
    KFileItem fileItem(url, KFileItem::NormalMimeTypeDetermination);
    return fileItem.mimetype();
}

QString MiscUtils::md5(const QString &str)
{
    auto md5 = QCryptographicHash::hash((str.toUtf8()), QCryptographicHash::Md5);

    return QString::fromUtf8(md5.toHex());
}

std::optional<Metadata> MiscUtils::metadata(const QUrl &url)
{
    QString mimeType = MiscUtils::mimeType(url);

    using namespace KFileMetaData;
    ExtractorCollection exCol;
    QList<Extractor *> extractors = exCol.fetchExtractors(mimeType);
    if (extractors.isEmpty()) {
        return {};
    }
    SimpleExtractionResult result(url.toLocalFile(), mimeType, ExtractionResult::ExtractMetaData);

    Extractor *ex = extractors.first();
    ex->extract(&result);

    Metadata m;
    m.url = url;
    m.imageData = result.imageData();
    m.properties = result.properties();

    return {m};
}

// #include "moc_miscutils.h"
