/*
 * SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "file.h"

File::File(const QString &path)
    : m_url{QUrl(path)}
{
    if (m_url.scheme() == QStringLiteral("http") || m_url.scheme() == QStringLiteral("https")) {
        m_path = path;
        m_type = Http;
    } else if (m_url.scheme() == QStringLiteral("file")) {
        auto pathWithoutScheme = QString(path).remove(0, QString("file://").length());
        m_path = QUrl::fromPercentEncoding(pathWithoutScheme.toUtf8());
        m_type = Local;
    } else if (m_url.scheme().isEmpty()) {
        m_path = QUrl::fromPercentEncoding(path.toUtf8());
        m_type = Local;
    } else {
        m_path = path;
        m_type = Unknown;
    }
}

File::Type File::type()
{
    return m_type;
}

QString File::path()
{
    return m_path;
}
