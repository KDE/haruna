
/*
 * SPDX-FileCopyrightText: 2021 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef GLOBALS_H
#define GLOBALS_H

#include <QStandardPaths>
#include <QtQml/qqmlregistration.h>

#include <KSharedConfig>

class QQmlEngine;
class QJSEngine;

class Global : public QObject
{
    Q_OBJECT
    QML_SINGLETON
    QML_ELEMENT

public:
    static Global *instance();
    static Global *create(QQmlEngine *, QJSEngine *);

    enum class ConfigFile {
        Main,
        CustomCommands,
        Shortcuts,
        Database,
        PlaylistCache,
    };
    Q_ENUM(ConfigFile)

    Q_INVOKABLE const QString configFileParentPath(ConfigFile configFile = ConfigFile::Main);
    Q_INVOKABLE const QString configFilePath(ConfigFile configFile = ConfigFile::Main);
    Q_INVOKABLE const QString playlistsFolder();

Q_SIGNALS:
    void error(const QString &message);

private:
    Global();
    ~Global() = default;

    Global(const Global &) = delete;
    Global &operator=(const Global &) = delete;
    Global(Global &&) = delete;
    Global &operator=(Global &&) = delete;

    KSharedConfig::Ptr m_config;
    KSharedConfig::Ptr m_ccConfig;
    KSharedConfig::Ptr m_shortcutsConfig;
};

#endif // GLOBALS_H
