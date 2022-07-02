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

#include <KRecentFilesAction>
#include <KSharedConfig>

RecentFilesModel::RecentFilesModel(QObject *parent)
    : QAbstractListModel(parent)
{
    auto config = KSharedConfig::openConfig(Global::instance()->appConfigFilePath());
    m_recentFilesConfigGroup = config->group(QStringLiteral("RecentFiles"));
    m_recentFilesAction = new KRecentFilesAction(this);
    m_recentFilesAction->setMaxItems(GeneralSettings::maxRecentFiles());
    m_recentFilesAction->loadEntries(m_recentFilesConfigGroup);

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
    beginResetModel();
    m_urls.clear();
    endResetModel();

    m_recentFilesAction->setMaxItems(GeneralSettings::maxRecentFiles());
    for (int i = 0; i < m_recentFilesAction->maxItems(); i++) {
        auto file = m_recentFilesConfigGroup.readPathEntry(QStringLiteral("File%1").arg(i + 1), QString());
        auto name = m_recentFilesConfigGroup.readPathEntry(QStringLiteral("Name%1").arg(i + 1), QString());
        if (file.isEmpty()) {
            break;
        }
        beginInsertRows(QModelIndex(), 0, 0);
        RecentFile recentFile;
        recentFile.url = QUrl(file);
        recentFile.name = name;
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
    m_recentFilesAction->addUrl(url, _name);
    m_recentFilesAction->saveEntries(m_recentFilesConfigGroup);
    m_recentFilesConfigGroup.sync();

    populate();
}

void RecentFilesModel::clear()
{
    beginResetModel();
    for (const auto &url : m_recentFilesAction->urls()) {
        m_recentFilesAction->removeUrl(url);
    }
    m_recentFilesAction->saveEntries(m_recentFilesConfigGroup);
    m_recentFilesConfigGroup.sync();
    m_urls.clear();
    endResetModel();
}
