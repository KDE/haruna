/*
 * SPDX-FileCopyrightText: 2021 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "recentfilesmodel.h"

#include <KSharedConfig>

#include <QJsonDocument>
#include <QMenu>
#include <QProcess>

#include "application.h"
#include "generalsettings.h"
#include "global.h"

RecentFilesModel::RecentFilesModel(QObject *parent)
    : QAbstractListModel(parent)
{
    auto config = KSharedConfig::openConfig(Global::instance()->appConfigFilePath(Global::ConfigFile::RecentFiles));
    m_recentFilesConfigGroup = config->group(QStringLiteral("RecentFiles"));
    setMaxRecentFiles(GeneralSettings::maxRecentFiles());
    populate();
}

int RecentFilesModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return m_urls.count();
}

QVariant RecentFilesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    RecentFile recentFile = m_urls.at(index.row());

    switch (role) {
    case PathRole:
        return QVariant(recentFile.url);
    case NameRole:
        return QVariant(recentFile.name);
    }

    return QVariant();
}

QHash<int, QByteArray> RecentFilesModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[PathRole] = "path";
    roles[NameRole] = "name";

    return roles;
}

void RecentFilesModel::populate()
{
    if (GeneralSettings::maxRecentFiles() <= 0) {
        deleteEntries();
        return;
    }

    clear();
    setMaxRecentFiles(GeneralSettings::maxRecentFiles());

    for (int i = 0; i < maxRecentFiles(); i++) {
        auto file = m_recentFilesConfigGroup.readPathEntry(QStringLiteral("File%1").arg(i + 1), QString());
        auto name = m_recentFilesConfigGroup.readPathEntry(QStringLiteral("Name%1").arg(i + 1), QString());
        if (file.isEmpty()) {
            break;
        }
        QUrl url(file);
        if (!url.isLocalFile() && url.scheme().isEmpty()) {
            url.setScheme(QStringLiteral("file"));
        }
        RecentFile recentFile;
        recentFile.url = url;
        recentFile.name = name;

        beginInsertRows(QModelIndex(), i, i);
        m_urls.append(recentFile);
        endInsertRows();
    }
}

void RecentFilesModel::addUrl(const QString &path, const QString &name)
{
    if (maxRecentFiles() == 0) {
        return;
    }

    auto url = QUrl::fromUserInput(path);
    if (!url.isValid()) {
        return;
    }

    if (url.scheme().startsWith(QStringLiteral("http")) && name.isEmpty()) {
        getHttpItemInfo(url);
    }

    auto _name = name == QString() ? url.fileName() : name;

    for (int i{0}; i < m_urls.count(); ++i) {
        if (url == m_urls[i].url) {
            beginRemoveRows(QModelIndex(), i, i);
            m_urls.takeAt(i);
            endRemoveRows();
            break;
        }
    }

    RecentFile recentFile;
    recentFile.url = url;
    recentFile.name = _name;

    if (m_urls.count() == maxRecentFiles()) {
        beginRemoveRows(QModelIndex(), m_urls.count() - 1, m_urls.count() - 1);
        m_urls.removeLast();
        endRemoveRows();
    }

    beginInsertRows(QModelIndex(), 0, 0);
    m_urls.prepend(recentFile);
    endInsertRows();

    saveEntries();
}

void RecentFilesModel::clear()
{
    beginResetModel();
    m_urls.clear();
    endResetModel();
}

void RecentFilesModel::deleteEntries()
{
    clear();
    m_recentFilesConfigGroup.deleteGroup();
    m_recentFilesConfigGroup.sync();
}

void RecentFilesModel::saveEntries()
{
    m_recentFilesConfigGroup.deleteGroup();
    int i = 1;
    for (const auto &[url, name] : std::as_const(m_urls)) {
        m_recentFilesConfigGroup.writePathEntry(QStringLiteral("File%1").arg(i), url.toDisplayString(QUrl::PreferLocalFile));
        m_recentFilesConfigGroup.writePathEntry(QStringLiteral("Name%1").arg(i), name);

        ++i;
    }
    m_recentFilesConfigGroup.sync();
}

int RecentFilesModel::maxRecentFiles() const
{
    return m_maxRecentFiles;
}

void RecentFilesModel::setMaxRecentFiles(int _maxRecentFiles)
{
    m_maxRecentFiles = _maxRecentFiles;
}

void RecentFilesModel::getHttpItemInfo(const QUrl &url)
{
    auto ytdlProcess = std::make_shared<QProcess>();
    ytdlProcess->setProgram(Application::youtubeDlExecutable());
    ytdlProcess->setArguments(QStringList() << QStringLiteral("-j") << url.toString());
    ytdlProcess->start();

    connect(ytdlProcess.get(), QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [=](int, QProcess::ExitStatus) {
        QString json = QString::fromUtf8(ytdlProcess->readAllStandardOutput());
        QString title = QJsonDocument::fromJson(json.toUtf8())[QStringLiteral("title")].toString();
        if (title.isEmpty()) {
            return;
        }

        addUrl(url.toString(), title);
    });
}

#include "moc_recentfilesmodel.cpp"
