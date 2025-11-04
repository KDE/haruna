/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef APPLICATION_H
#define APPLICATION_H

#include <QObject>
#include <QQmlPropertyMap>
#include <QtQml/qqmlregistration.h>

#include <KAboutData>

class QAbstractItemModel;
class QApplication;
class QJSEngine;
class QQmlApplicationEngine;
class QQmlEngine;
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
    bool eventFilter(QObject *obj, QEvent *event) override;
};

class Application : public QObject
{
    Q_OBJECT
    QML_SINGLETON
    QML_NAMED_ELEMENT(HarunaApp)

public:
    static Application *instance();
    static Application *create(QQmlEngine *, QJSEngine *);

    Q_PROPERTY(QAbstractItemModel *colorSchemesModel READ colorSchemesModel CONSTANT)
    Q_PROPERTY(QQmlPropertyMap *actions MEMBER m_actions CONSTANT)
    Q_PROPERTY(bool actionsEnabled READ actionsEnabled WRITE setActionsEnabled NOTIFY actionsEnabledChanged)
    bool actionsEnabled();
    void setActionsEnabled(bool enable);

    Q_INVOKABLE QUrl url(int key);
    Q_INVOKABLE void addUrl(int key, const QString &value);
    Q_INVOKABLE QStringList availableGuiStyles();
    Q_INVOKABLE void setGuiStyle(const QString &style);
    Q_INVOKABLE void activateColorScheme(const QString &name);
    Q_INVOKABLE int frameworksVersionMinor();
    Q_INVOKABLE int qtMajorVersion();
    Q_INVOKABLE void raiseWindow();

    static QString version();

    void handleSecondayInstanceMessage(const QByteArray &message, const QString activationToken);
    QCommandLineParser *parser() const;

    QQmlApplicationEngine *qmlEngine() const;
    void setQmlEngine(QQmlApplicationEngine *_qmlEngine);

Q_SIGNALS:
    void qmlApplicationMouseLeave();
    void qmlApplicationMouseEnter();
    void openUrl(const QUrl &url);
    void actionsEnabledChanged();

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
    std::unique_ptr<QCommandLineParser> m_parser;
    QMap<int, QUrl> m_urls;
    KColorSchemeManager *m_schemes{nullptr};
    QString m_systemDefaultStyle;
    QQmlApplicationEngine *m_qmlEngine{nullptr};
    std::unique_ptr<ApplicationEventFilter> m_appEventFilter;
    QQmlPropertyMap *m_actions{new QQmlPropertyMap};
    bool m_actionsEnabled{true};
};

#endif // APPLICATION_H
