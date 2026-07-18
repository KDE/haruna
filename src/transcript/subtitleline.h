/*
 * SPDX-FileCopyrightText: 2026 Muhammet Sadık Uğursoy <sadikugursoy@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef SUBTITLELINE_H
#define SUBTITLELINE_H

#include <QString>

struct SubtitleLine {
    QString text;
    double startTime;
    double endTime;
    QString formattedStartTime;
    QString formattedEndTime;
    double duration;
};

#endif // SUBTITLELINE_H
