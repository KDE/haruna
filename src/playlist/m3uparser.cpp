/*
 * SPDX-FileCopyrightText: 2026 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "m3uparser.h"

#include <QDebug>
#include <QDir>
#include <QFile>

using namespace Qt::StringLiterals;

M3uParser::M3uParser()
{
}

void M3uParser::read(const std::filesystem::path &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Couldn't open file:" << path.u16string() << file.errorString();
        return;
    }

    auto firstLine = file.readLine();
    if (firstLine.startsWith("#EXTM3U")) {
        while (!file.atEnd()) {
            auto line = file.readLine();
            parseExtendedLine(line);
        }
    } else {
        parseStandardLine(firstLine);
        while (!file.atEnd()) {
            auto line = file.readLine();
            parseStandardLine(line);
        }
    }
}

void M3uParser::write(const PlaylistFilterProxyModel *playlistModel, const std::filesystem::path &savePath)
{
    QFileInfo fileInfo(savePath);
    QDir dir = fileInfo.dir();
    if (!dir.exists()) {
        if (!dir.mkpath(dir.absolutePath())) {
            qDebug() << "Failed to create playlist directory:" << dir.path();
            return;
        }
    }

    QFile m3uFile(savePath);
    if (!m3uFile.open(QFile::WriteOnly)) {
        qDebug() << "Couldn't open file:" << savePath << m3uFile.errorString();
        return;
    }

    m3uFile.write("#EXTM3U\n");
    for (int i{0}; i < playlistModel->rowCount(); ++i) {
        const auto itemPath = playlistModel->data(playlistModel->index(i, 0), PlaylistModel::PathRole).toString();
        const auto title = playlistModel->data(playlistModel->index(i, 0), PlaylistModel::TitleRole).toString();
        const auto duration = playlistModel->data(playlistModel->index(i, 0), PlaylistModel::DurationRole).toDouble();

        if (duration > 0) {
            m3uFile.write(u"#EXTINF:%1,%2\n"_s.arg(duration).arg(title).toUtf8());
        }
        m3uFile.write(itemPath.toUtf8().append("\n"));
    }

    m3uFile.close();
}

void M3uParser::parseStandardLine(QByteArray &line)
{
    while (line.endsWith('\n') || line.endsWith('\r')) {
        line.chop(1);
    }

    if (line.isEmpty()) {
        return;
    }

    // ignore comments
    if (line.startsWith("#")) {
        return;
    }

    metadata.path = QString::fromUtf8(line);
    m_data.append(metadata);
}

void M3uParser::parseExtendedLine(QByteArray &line)
{
    while (line.endsWith('\n') || line.endsWith('\r')) {
        line.chop(1);
    }

    if (line.isEmpty()) {
        return;
    }

    if (!line.startsWith("#")) {
        metadata.path = QString::fromUtf8(line);
        m_data.append(std::move(metadata));
        metadata.duration.reset();
        metadata.title.reset();
        metadata.path.clear();
        return;
    }

    QString key;
    QString value;
    bool colonFound{false};

    for (const auto &c : std::as_const(line)) {
        if (c == u':') {
            colonFound = true;
            continue;
        }

        if (!colonFound) {
            key += QChar::fromLatin1(c);
        } else {
            value += QChar::fromLatin1(c);
        }
    }

    if (key == u"#EXTINF") {
        auto i = value.indexOf(u',');
        if (i == -1) {
            metadata.duration = value.toDouble();
        } else {
            metadata.duration = value.sliced(0, i).toDouble();
            metadata.title = value.sliced(i + 1).simplified();
        }
    }
}

QList<MetaData> M3uParser::data() const
{
    return m_data;
}
