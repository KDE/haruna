/*
 * SPDX-FileCopyrightText: 2021 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "recentfilesmodel.h"

#include "generalsettings.h"
#include <global.h>

#include <QDebug>
#include <QMenu>

#include <KSharedConfig>

RecentFilesModel::RecentFilesModel(QObject *parent)
    : QAbstractListModel(parent)
{
    auto config = KSharedConfig::openConfig(Global::instance()->appConfigFilePath());
    m_recentFilesConfigGroup = config->group(QStringLiteral("RecentFiles"));
    setMaxRecentFiles(GeneralSettings::maxRecentFiles());
    populate();
}

int RecentFilesModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_urls.count();
}

QVariant RecentFilesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

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
    clear();
    setMaxRecentFiles(GeneralSettings::maxRecentFiles());

    for (int i = 0; i < maxRecentFiles(); i++) {
        auto file = m_recentFilesConfigGroup.readPathEntry(QStringLiteral("File%1").arg(i + 1), QString());
        auto name = m_recentFilesConfigGroup.readPathEntry(QStringLiteral("Name%1").arg(i + 1), QString());
        if (file.isEmpty()) {
            break;
        }
        RecentFile recentFile;
        recentFile.url = QUrl(file);
        recentFile.name = name;
        beginInsertRows(QModelIndex(), 0, 0);
        m_urls.prepend(recentFile);
        endInsertRows();
    }
}

void RecentFilesModel::addUrl(const QString &path, const QString &name)
{
    auto config = KSharedConfig::openConfig(Global::instance()->appConfigFilePath());
    QUrl url(path);
    if (!url.isLocalFile() && url.scheme().isEmpty()) {
        url.setScheme("file");
    }

    auto _name = name == QString() ? url.fileName() : name;
    RecentFile recentFile;
    recentFile.url = url;
    recentFile.name = _name;
    beginInsertRows(QModelIndex(), 0, 0);
    m_urls.prepend(recentFile);
    endInsertRows();

    m_recentFilesConfigGroup.writePathEntry(QStringLiteral("File%1").arg(m_urls.count()),
                                            url.toDisplayString(QUrl::PreferLocalFile));
    m_recentFilesConfigGroup.writePathEntry(QStringLiteral("Name%1").arg(m_urls.count()), _name);
    m_recentFilesConfigGroup.sync();
}

void RecentFilesModel::clear()
{
    beginResetModel();
    m_urls.clear();
    endResetModel();
}

int RecentFilesModel::maxRecentFiles() const
{
    return m_maxRecentFiles;
}

void RecentFilesModel::setMaxRecentFiles(int _maxRecentFiles)
{
    m_maxRecentFiles = _maxRecentFiles;
}
