/*
 * SPDX-FileCopyrightText: 2025 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef RECENTFILE_H
#define RECENTFILE_H

#include <QString>
#include <QUrl>

struct RecentFile {
    uint id{0};
    QUrl url;
    QString filename;
    QString openedFrom;
    qint64 timestamp{0};
};

#endif // RECENTFILE_H
