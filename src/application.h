/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef APPLICATION_H
#define APPLICATION_H

#include <QAbstractItemModel>
#include <QApplication>
#include <QFont>
#include <QObject>
#include <QQmlApplicationEngine>

#include <KAboutData>
#include <KSharedConfig>

class QQuickWindow;
class KActionCollection;
class KConfigDialog;
class KColorSchemeManager;

class ApplicationEventFilter : public QObject
{
    Q_OBJECT

Q_SIGNALS:
    void applicationMouseLeave();
    void applicationMouseEnter();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override
    {
        if (event->type() == QEvent::Leave) {
            Q_EMIT applicationMouseLeave();
        }
        if (event->type() == QEvent::Enter) {
            Q_EMIT applicationMouseEnter();
        }
        return QObject::eventFilter(obj, event);
    }
};

class Application : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel *colorSchemesModel READ colorSchemesModel CONSTANT)

public:
    static Application *instance();

    Q_INVOKABLE bool urlExists(const QUrl &url);
    Q_INVOKABLE bool pathExists(const QString &url);
    Q_INVOKABLE QUrl configFilePath();
    Q_INVOKABLE QUrl ccConfigFilePath();
    Q_INVOKABLE QUrl configFolderPath();
    Q_INVOKABLE QUrl pathToUrl(const QString &path);
    Q_INVOKABLE void restoreWindowGeometry(QQuickWindow *window) const;
    Q_INVOKABLE void saveWindowGeometry(QQuickWindow *window);
    Q_INVOKABLE bool configFolderExists();
    Q_INVOKABLE QUrl parentUrl(const QString &path);
    Q_INVOKABLE QUrl url(int key);
    Q_INVOKABLE void addUrl(int key, const QString &value);
    Q_INVOKABLE QString getFileContent(const QString &file);
    Q_INVOKABLE QStringList availableGuiStyles();
    Q_INVOKABLE void setGuiStyle(const QString &style);
    Q_INVOKABLE void activateColorScheme(const QString &name);
    Q_INVOKABLE QStringList getFonts();
    Q_INVOKABLE int frameworksVersionMinor();
    Q_INVOKABLE int qtMajorVersion();
    Q_INVOKABLE QString platformName();
    Q_INVOKABLE void raiseWindow();

    static QString version();
    Q_INVOKABLE static bool hasYoutubeDl();
    Q_INVOKABLE static QString youtubeDlExecutable();
    Q_INVOKABLE static bool isYoutubePlaylist(const QString &path);
    Q_INVOKABLE static QString formatTime(const double time);
    Q_INVOKABLE static QString mimeType(QUrl url);

    void handleSecondayInstanceMessage(const QByteArray &message);
    QCommandLineParser *parser() const;

    QQmlApplicationEngine *qmlEngine() const;
    void setQmlEngine(QQmlApplicationEngine *_qmlEngine);

Q_SIGNALS:
    void qmlApplicationMouseLeave();
    void qmlApplicationMouseEnter();
    void error(const QString &message);
    void saveWindowGeometryAsync(QQuickWindow *window);
    void openUrl(const QUrl &url);

private:
    explicit Application();
    ~Application();

    Application(const Application &) = delete;
    Application &operator=(const Application &) = delete;
    Application(Application &&) = delete;
    Application &operator=(Application &&) = delete;

    void setupWorkerThread();
    void setupAboutData();
    void setupCommandLineParser();
    void setupQmlSettingsTypes();
    QAbstractItemModel *colorSchemesModel();
    QApplication *m_app{nullptr};
    KAboutData m_aboutData;
    KSharedConfig::Ptr m_config;
    std::unique_ptr<QCommandLineParser> m_parser;
    QMap<int, QUrl> m_urls;
    KColorSchemeManager *m_schemes{nullptr};
    QString m_systemDefaultStyle;
    QQmlApplicationEngine *m_qmlEngine{nullptr};
    std::unique_ptr<ApplicationEventFilter> m_appEventFilter;
};

#endif // APPLICATION_H
