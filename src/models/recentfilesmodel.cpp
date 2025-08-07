/*
 * SPDX-FileCopyrightText: 2021 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "recentfilesmodel.h"

#include <QJsonDocument>
#include <QJsonValue>
#include <QProcess>
#include <QUrlQuery>

#include "application.h"
#include "database.h"
#include "generalsettings.h"

using namespace Qt::StringLiterals;

// clang-format off
const QString OpenAction  = u"OpenAction"_s;
const QString ExternalApp = u"ExternalApp"_s;
const QString Playlist    = u"Playlist"_s;
const QString Other       = u"Other"_s;
// clang-format on

RecentFilesModel::RecentFilesModel(QObject *parent)
    : QAbstractListModel(parent)
{
    getItems();
}

int RecentFilesModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return m_data.count();
}

QVariant RecentFilesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    const auto item = m_data.at(index.row());

    switch (role) {
    case UrlRole:
        return QVariant(item.url);
    case NameRole:
        return QVariant(item.filename);
    case OpenedFromRole:
        return QVariant::fromValue(stringToOpenedFrom(item.openedFrom));
    }

    return QVariant();
}

QHash<int, QByteArray> RecentFilesModel::roleNames() const
{
    QHash<int, QByteArray> roles{
        {UrlRole, QByteArrayLiteral("url")},
        {NameRole, QByteArrayLiteral("filename")},
        {OpenedFromRole, QByteArrayLiteral("openedFrom")},
    };

    return roles;
}

void RecentFilesModel::getItems()
{
    const auto maxFiles{GeneralSettings::maxRecentFiles()};
    if (maxFiles <= 0) {
        return;
    }

    clear();

    const auto recentFiles = Database::instance()->recentFiles(GeneralSettings::maxRecentFiles());
    beginInsertRows(QModelIndex(), 0, recentFiles.count() - 1);
    m_data = recentFiles;
    endInsertRows();
}

void RecentFilesModel::addRecentFile(const QUrl &url, OpenedFrom openedFrom, const QString &name)
{
    const auto maxFiles{GeneralSettings::maxRecentFiles()};
    if (maxFiles <= 0) {
        return;
    }

    if (url.scheme().isEmpty()) {
        qDebug() << "No scheme for:" << url << openedFrom;
        return;
    }

    if (url.scheme().startsWith(u"http"_s) && name.isEmpty()) {
        if (!url.toString().contains(u"list="_s)) {
            getHttpItemInfo(url, openedFrom);
        }
        return;
    }

    int i{0};
    for (const auto &rf : std::as_const(m_data)) {
        if (rf.url == url) {
            auto item = m_data.takeAt(i);
            item.timestamp = QDateTime::currentMSecsSinceEpoch();
            m_data.prepend(item);
            Q_EMIT dataChanged(index(0, 0), index(i, 0));
            Database::instance()->addRecentFile(item.url, item.filename, item.openedFrom, item.timestamp);
            return;
        }
        i++;
    }

    RecentFile rf;
    rf.url = url;
    rf.filename = name.isEmpty() ? url.fileName() : name;
    rf.openedFrom = openedFromToString(openedFrom);
    rf.timestamp = QDateTime::currentMSecsSinceEpoch();

    beginInsertRows(QModelIndex(), 0, 0);
    m_data.prepend(rf);
    endInsertRows();

    Database::instance()->addRecentFile(rf.url, rf.filename, rf.openedFrom, rf.timestamp);
}

void RecentFilesModel::clear()
{
    beginResetModel();
    m_data.clear();
    endResetModel();
}

void RecentFilesModel::deleteEntries()
{
    clear();
    Database::instance()->deleteRecentFiles();
}

void RecentFilesModel::getHttpItemInfo(const QUrl &url, OpenedFrom openedFrom)
{
    QUrlQuery query{url.query()};
    QString playlistId{query.queryItemValue(u"list"_s)};
    QString videoId{query.queryItemValue(u"v"_s)};

    QUrl urlWithoutPlaylist = url;
    if ((url.host() == u"www.youtube.com"_s || url.host() == u"youtu.be"_s) && !playlistId.isEmpty()) {
        urlWithoutPlaylist = QUrl{u"https://www.youtube.com/watch?v=%1"_s.arg(videoId)};
    }

    auto ytdlProcess = std::make_shared<QProcess>();
    ytdlProcess->setProgram(Application::youtubeDlExecutable());
    ytdlProcess->setArguments(QStringList() << u"-j"_s << urlWithoutPlaylist.toString());
    ytdlProcess->start();

    connect(ytdlProcess.get(), QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [=](int, QProcess::ExitStatus) {
        QString json = QString::fromUtf8(ytdlProcess->readAllStandardOutput());
        QString title = QJsonDocument::fromJson(json.toUtf8())[u"title"_s].toString();
        if (title.isEmpty()) {
            return;
        }

        addRecentFile(url, openedFrom, title);
    });
}

QString RecentFilesModel::openedFromToString(OpenedFrom from) const
{
    switch (from) {
    case OpenedFrom::OpenAction:
        return OpenAction;
    case OpenedFrom::ExternalApp:
        return ExternalApp;
    case OpenedFrom::Playlist:
        return Playlist;
    case OpenedFrom::Other:
        return QString{};
    }

    return QString{};
}

RecentFilesModel::OpenedFrom RecentFilesModel::stringToOpenedFrom(const QString &from) const
{
    if (from == OpenAction) {
        return OpenedFrom::OpenAction;
    }
    if (from == ExternalApp) {
        return OpenedFrom::ExternalApp;
    }
    if (from == Playlist) {
        return OpenedFrom::Playlist;
    }
    if (from == Other) {
        return OpenedFrom::Other;
    }

    return OpenedFrom::Other;
}

#include "moc_recentfilesmodel.cpp"
