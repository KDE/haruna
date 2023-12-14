/*
 * SPDX-FileCopyrightText: 2021 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "global.h"

#include <QFileInfo>

Global *Global::instance()
{
    static Global g;
    return &g;
}

Global::Global()
    : m_config(KSharedConfig::openConfig(u"haruna/haruna.conf"_qs, KConfig::SimpleConfig, QStandardPaths::GenericConfigLocation))
    , m_ccConfig(KSharedConfig::openConfig(u"haruna/custom-commands.conf"_qs, KConfig::SimpleConfig, QStandardPaths::GenericConfigLocation))
    , m_rfConfig(KSharedConfig::openConfig(u"haruna/recent-files.conf"_qs, KConfig::SimpleConfig, QStandardPaths::GenericDataLocation))
    , m_shortcutsConfig(KSharedConfig::openConfig(u"haruna/shortcuts.conf"_qs, KConfig::SimpleConfig, QStandardPaths::GenericConfigLocation))
{
}

const QString Global::appConfigDirPath()
{
    auto path = QStandardPaths::writableLocation(m_config->locationType()).append(u"/haruna"_qs);
    QFileInfo configFolder(path);
    if (configFolder.exists()) {
        return configFolder.absoluteFilePath();
    }
    return QString();
}

const QString Global::appConfigFilePath(ConfigFile configFile)
{
    switch (configFile) {
    case ConfigFile::Main: {
        auto path = QStandardPaths::writableLocation(m_config->locationType()).append(u"/"_qs);
        return path.append(m_config->name());
    }
    case ConfigFile::CustomCommands: {
        auto path = QStandardPaths::writableLocation(m_ccConfig->locationType()).append(u"/"_qs);
        return path.append(m_ccConfig->name());
    }
    case ConfigFile::RecentFiles: {
        auto path = QStandardPaths::writableLocation(m_rfConfig->locationType()).append(u"/"_qs);
        return path.append(m_rfConfig->name());
    }
    case ConfigFile::Shortcuts: {
        auto path = QStandardPaths::writableLocation(m_shortcutsConfig->locationType()).append(u"/"_qs);
        return path.append(m_shortcutsConfig->name());
    }
    default:
        return QString();
    }
}

#include "moc_global.cpp"
