
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

class PathUtils : public QObject
{
    Q_OBJECT
    QML_SINGLETON
    QML_ELEMENT

public:
    static PathUtils *instance();
    static PathUtils *create(QQmlEngine *, QJSEngine *);

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
    Q_INVOKABLE void highlightInFileManager(const QString &path);

    Q_INVOKABLE bool pathExists(const QString &path);
    Q_INVOKABLE QUrl pathToUrl(const QString &path);
    Q_INVOKABLE QString urlToPath(const QUrl &url);
    Q_INVOKABLE QString parentPath(const QString &path);
    Q_INVOKABLE QUrl parentUrl(const QUrl &url);

Q_SIGNALS:
    void error(const QString &message);

private:
    PathUtils();
    ~PathUtils() = default;

    PathUtils(const PathUtils &) = delete;
    PathUtils &operator=(const PathUtils &) = delete;
    PathUtils(PathUtils &&) = delete;
    PathUtils &operator=(PathUtils &&) = delete;

    KSharedConfig::Ptr m_config;
    KSharedConfig::Ptr m_ccConfig;
    KSharedConfig::Ptr m_shortcutsConfig;
};

#endif // GLOBALS_H
