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
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Couldn't open file:" << path.u16string() << file.errorString();
        return;
    }

    QTextStream in(&file);

    if (in.atEnd()) {
        return;
    }

    auto firstLine = in.readLine();
    if (firstLine.startsWith(u"#EXTM3U")) {
        while (!in.atEnd()) {
            parseExtendedLine(in.readLine(), QString::fromStdString(path));
        }
    } else {
        parseStandardLine(firstLine, QString::fromStdString(path));
        while (!in.atEnd()) {
            parseStandardLine(in.readLine(), QString::fromStdString(path));
        }
    }
    file.close();
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
    if (!m3uFile.open(QFile::WriteOnly | QIODevice::Text)) {
        qDebug() << "Couldn't open file:" << savePath << m3uFile.errorString();
        return;
    }

    QTextStream out(&m3uFile);
    out.setEncoding(QStringConverter::Utf8);

    out << "#EXTM3U\n";
    for (int i{0}; i < playlistModel->rowCount(); ++i) {
        const auto itemPath = playlistModel->data(playlistModel->index(i, 0), PlaylistModel::PathRole).toString();
        const auto title = playlistModel->data(playlistModel->index(i, 0), PlaylistModel::TitleRole).toString();
        const auto duration = playlistModel->data(playlistModel->index(i, 0), PlaylistModel::DurationRole).toDouble();

        if (duration > 0) {
            out << u"#EXTINF:%1,%2\n"_s.arg(duration).arg(title);
        }
        out << itemPath << "\n";
    }

    m3uFile.close();
}

void M3uParser::parseStandardLine(const QString &line, const QString &playlistPath)
{
    if (line.isEmpty() || line.startsWith(u"#")) {
        return;
    }

    metadata.url = stringToUrl(line, playlistPath);
    m_data.append(metadata);
}

void M3uParser::parseExtendedLine(const QString &line, const QString &playlistPath)
{
    if (line.isEmpty()) {
        return;
    }

    if (!line.startsWith(u"#")) {
        metadata.url = stringToUrl(line, playlistPath);
        m_data.append(std::move(metadata));
        metadata.duration.reset();
        metadata.title.reset();
        metadata.url.clear();
        return;
    }

    QString key;
    QString value;
    bool colonFound{false};
    for (const auto &c : std::as_const(line)) {
        if (c == u':' && !colonFound) {
            colonFound = true;
            continue;
        }

        if (!colonFound) {
            key += c;
        } else {
            value += c;
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

QUrl M3uParser::stringToUrl(const QString &urlString, const QString &parentPath)
{
    const QString decoded = QUrl::fromPercentEncoding(urlString.toUtf8());

    const QUrl url(decoded);
    if (url.isValid() && !url.scheme().isEmpty()) {
        return url;
    }

    std::filesystem::path path(decoded.toStdString());
    std::filesystem::path playlistParentPath(parentPath.toStdString());

    if (path.is_relative()) {
        path = playlistParentPath.parent_path() / path;
    }

    return QUrl::fromLocalFile(QString::fromStdString(path));
}

QList<MetaData> M3uParser::data() const
{
    return m_data;
}

#include "moc_m3uparser.cpp"
