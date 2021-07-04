/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef APPLICATION_H
#define APPLICATION_H

#include <QAbstractItemModel>
#include <QAction>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QObject>

#include <KAboutData>
#include <KActionCollection>
#include <KSharedConfig>

class KActionCollection;
class KConfigDialog;
class KColorSchemeManager;
class QAction;

class Application : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel* colorSchemesModel READ colorSchemesModel CONSTANT)
    Q_PROPERTY(QUrl configFilePath READ configFilePath CONSTANT)
    Q_PROPERTY(QUrl configFolderPath READ configFolderPath CONSTANT)

public:
    explicit Application(int &argc, char **argv, const QString &applicationName);
    ~Application();

    int run();
    QUrl configFilePath();
    QUrl configFolderPath();
    Q_INVOKABLE QUrl parentUrl(const QString &path);
    Q_INVOKABLE QUrl pathToUrl(const QString &path);
    Q_INVOKABLE QString argument(int key);
    Q_INVOKABLE void addArgument(int key, const QString &value);
    Q_INVOKABLE QAction *action(const QString &name);
    Q_INVOKABLE QString getFileContent(const QString &file);
    Q_INVOKABLE QString mimeType(const QString &file);
    Q_INVOKABLE QStringList availableGuiStyles();
    Q_INVOKABLE void setGuiStyle(const QString &style);
    Q_INVOKABLE void activateColorScheme(const QString &name);
    Q_INVOKABLE void configureShortcuts(const QString &name = QString());
    Q_INVOKABLE void createUserAction(const QString &text);
    Q_INVOKABLE void openDocs(const QString &page);

    static QString version();
    Q_INVOKABLE static bool hasYoutubeDl();
    Q_INVOKABLE static bool isYoutubePlaylist(const QString &path);
    Q_INVOKABLE static QString formatTime(const double time);
    Q_INVOKABLE static void hideCursor();
    Q_INVOKABLE static void showCursor();

private:
    void setupWorkerThread();
    void setupAboutData();
    void setupCommandLineParser();
    void registerQmlTypes();
    void setupQmlSettingsTypes();
    void setupQmlContextProperties();
    void aboutApplication();
    void setupActions(const QString &actionName);
    void setupUserActions();
    QAbstractItemModel *colorSchemesModel();
    QApplication *m_app;
    QQmlApplicationEngine *m_engine;
    KAboutData m_aboutData;
    KActionCollection m_collection;
    KSharedConfig::Ptr m_config;
    KConfigGroup *m_shortcuts;
    QMap<int, QString> m_args;
    KColorSchemeManager *m_schemes;
    QString m_systemDefaultStyle;
};

#endif // APPLICATION_H
