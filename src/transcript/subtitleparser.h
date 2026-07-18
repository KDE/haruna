/*
 * SPDX-FileCopyrightText: 2026 Muhammet Sadık Uğursoy <sadikugursoy@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef SUBTITLEPARSER_H
#define SUBTITLEPARSER_H

#include <QObject>

struct SubtitleLine;

class SubtitleParser : public QObject
{
    Q_OBJECT

public:
    explicit SubtitleParser(QObject *parent = nullptr);

    void parseSubtitle(const QUrl &url, const int streamIndex);

Q_SIGNALS:
    void transcriptItemReady(const SubtitleLine &item, const int streamIndex);

private:
    QString formatASS(const QString in);
};

#endif // SUBTITLEPARSER_H
