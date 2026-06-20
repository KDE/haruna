/*
 * SPDX-FileCopyrightText: 2026 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef M3UPARSER_H
#define M3UPARSER_H

#include <QMap>
#include <QString>

#include <filesystem>

#include "playlistfilterproxymodel.h"

struct MetaData {
    QString path;
    std::optional<double> duration;
    std::optional<QString> title;
};

class M3uParser
{
public:
    explicit M3uParser();
    void read(const std::filesystem::path &path);
    void write(const PlaylistFilterProxyModel *playlistModel, const std::filesystem::path &savePath);

    QList<MetaData> data() const;

private:
    void parseStandardLine(const QString &line);
    void parseExtendedLine(const QString &line);

    MetaData metadata;
    QList<MetaData> m_data;
};

#endif // M3UPARSER_H
