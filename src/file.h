/*
 * SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef FILE_H
#define FILE_H

#include <QString>
#include <QUrl>
/**
 * This class is needed because QUrl handling of local files containing
 * certain characters (#, ? etc.) is unsuitable for Haruna.
 *
 * Example:
 *     QUrl url("file:///video#1.webm");
 *     QUrl url2("/video#1.webm");
 *     qDebug() << url << url.toString(QUrl::PreferLocalFile) << url.toLocalFile();
 *     qDebug() << url2 << url2.toString(QUrl::PreferLocalFile) << url2.toLocalFile();
 *
 * prints:
 *     QUrl("file:///video#1.webm") "file:///video#1.webm" "/video"
 *     QUrl("/video#1.webm")        "/video#1.webm"        ""
 *
 * This class fixes the above by making sure the returned path is the same
 *     File file("file:///video#1.webm");
 *     File file2("/video#1.webm");
 *     qDebug() << "file 1" << file1.path();
 *     qDebug() << "file 2" << file2.path();
 *
 * prints:
 *     file 1 "/video#1.webm"
 *     file 2 "/video#1.webm"
 */
class File
{
public:
    enum Type {
        Local,
        Http,
        Unknown,
    };

    File(const QString &path);

    Type type();
    QString path();

private:
    Type m_type{Unknown};
    QString m_path{};
    QUrl m_url{};
};

#endif // FILE_H
