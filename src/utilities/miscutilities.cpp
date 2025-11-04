/*
 * SPDX-FileCopyrightText: 2025 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "miscutilities.h"

#include <KFileItem>

using namespace Qt::StringLiterals;

MiscUtilities::MiscUtilities(QObject *parent)
    : QObject{parent}
{

}

QString MiscUtilities::formatTime(const double time)
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

QString MiscUtilities::mimeType(QUrl url)
{
    KFileItem fileItem(url, KFileItem::NormalMimeTypeDetermination);
    return fileItem.mimetype();
}

// #include "moc_miscutilities.h"
