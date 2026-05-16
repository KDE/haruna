/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "application.h"

#include <QAbstractItemModel>
#include <QApplication>
#include <QCommandLineParser>
#include <QDesktopServices>
#include <QDir>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QProcess>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QStandardPaths>
#include <QStyle>
#include <QStyleFactory>
#include <QThread>
#include <QThreadPool>
#include <QUrlQuery>

#include "kconfig_version.h"
#include <KAboutData>
#include <KColorSchemeManager>
#include <KCrash>
#include <KFileItem>
#include <KLocalizedString>
#include <KWindowSystem>

#include "audiosettings.h"
#include "generalsettings.h"
#include "haruna-version.h"
#include "informationsettings.h"
#include "miscutils.h"
#include "mousesettings.h"
#include "pathutils.h"
#include "playbacksettings.h"
#include "playlistsettings.h"
#include "subtitlessettings.h"
#include "videosettings.h"
#include "worker.h"
#include "youtube.h"

using namespace Qt::StringLiterals;

bool ApplicationEventFilter::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::Leave) {
        Q_EMIT applicationMouseLeave();
    }
    if (event->type() == QEvent::Enter) {
        Q_EMIT applicationMouseEnter();
    }
    return QObject::eventFilter(obj, event);
}

Application *Application::instance()
{
    static Application app;
    return &app;
}

Application *Application::create(QQmlEngine *, QJSEngine *)
{
    QQmlEngine::setObjectOwnership(instance(), QQmlEngine::CppOwnership);
    return instance();
}

bool Application::actionsEnabled()
{
    return m_actionsEnabled;
}

void Application::setActionsEnabled(bool enable)
{
    if (enable == actionsEnabled()) {
        return;
    }
    m_actionsEnabled = enable;
    Q_EMIT actionsEnabledChanged();
}

Application::Application()
    : QObject{nullptr}
    , m_schemes(KColorSchemeManager::instance())
    , m_systemDefaultStyle(QApplication::style()->objectName())
    , m_appEventFilter{std::make_unique<ApplicationEventFilter>()}
{
    // used to hide playlist when mouse leaves the application
    // while moving between monitors while in fullscreen
    QApplication::instance()->installEventFilter(m_appEventFilter.get());
    QObject::connect(m_appEventFilter.get(), &ApplicationEventFilter::applicationMouseLeave, this, &Application::qmlApplicationMouseLeave);
    QObject::connect(m_appEventFilter.get(), &ApplicationEventFilter::applicationMouseEnter, this, &Application::qmlApplicationMouseEnter);

    if (GeneralSettings::guiStyle() != u"System"_s) {
        QApplication::setStyle(GeneralSettings::guiStyle());
    }

    setupWorkerThread();
    setupAboutData();
    setupCommandLineParser();

    KCrash::initialize();

    connect(PathUtils::instance(), &PathUtils::error, MiscUtils::instance(), &MiscUtils::error);
}

Application::~Application() = default;

void Application::setupWorkerThread()
{
    auto *worker = Worker::instance();
    auto *thread = new QThread();
    worker->moveToThread(thread);
    QObject::connect(thread, &QThread::finished, worker, &Worker::deleteLater);
    QObject::connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->start();
}

void Application::aboutDataAddComponent(const KAboutComponent &component)
{
    m_aboutData.addComponent(component);
}

void Application::setupAboutData()
{
    m_aboutData.setComponentName(u"haruna"_s);
    m_aboutData.setDisplayName(i18nc("application title/display name", "Haruna"));
    m_aboutData.setVersion(Application::version().toUtf8());
    m_aboutData.setShortDescription(i18nc("@title", "Media player"));
    m_aboutData.setLicense(KAboutLicense::GPL_V3);
    m_aboutData.setCopyrightStatement(i18nc("copyright statement, use copyright symbol and en dash for the year range", "© 2019–2025"));
    m_aboutData.setHomepage(u"https://haruna.kde.org"_s);
    m_aboutData.setBugAddress(u"https://bugs.kde.org/enter_bug.cgi?product=Haruna"_s.toUtf8());
    m_aboutData.setDesktopFileName(u"org.kde.haruna"_s);

    m_aboutData.addAuthor(i18nc("@info:credit", "George Florea Bănuș"),
                          i18nc("@info:credit", "Developer"),
                          u"georgefb899@gmail.com"_s,
                          u"https://georgefb.com"_s);

    QPointer self(this);
    QThreadPool::globalInstance()->start([self]() {
        if (!self) {
            return;
        }

        YouTube yt;
        if (yt.youtubeDlExecutable().isEmpty()) {
            return;
        }
        QProcess ytdlpProcess;
        ytdlpProcess.setProgram(yt.youtubeDlExecutable());
        ytdlpProcess.setArguments({u"--version"_s});
        ytdlpProcess.start();
        ytdlpProcess.waitForFinished(2000);

        auto ytdlpVersion = ytdlpProcess.readAllStandardOutput().simplified();
        QMetaObject::invokeMethod(self, [self, ytdlpVersion]() {
            if (!self) {
                return;
            }

            self->m_aboutData.addComponent(u"yt-dlp"_s,
                                           i18n("Feature-rich command-line audio/video downloader"),
                                           QString::fromUtf8(ytdlpVersion),
                                           u"https://github.com/yt-dlp/yt-dlp"_s,
                                           u"https://unlicense.org"_s);
            KAboutData::setApplicationData(self->m_aboutData);
        });
    });

    KAboutData::setApplicationData(m_aboutData);
}

void Application::setupCommandLineParser()
{
    m_parser = std::make_unique<QCommandLineParser>();
    m_aboutData.setupCommandLine(m_parser.get());
    m_parser->addPositionalArgument(u"file"_s, i18nc("@info:shell", "File to open"));

    QCommandLineOption ytdlFormatSelectionOption(QStringList() << u"ytdl-format-selection"_s << u"ytdlfs"_s,
                                                 i18nc("@info:shell",
                                                       "Allows to temporarily override the yt-dlp format selection setting. "
                                                       "Will be overwritten if the setting is changed through the GUI"),
                                                 u"bestvideo+bestaudio/best"_s,
                                                 QString());
    m_parser->addOption(ytdlFormatSelectionOption);

    m_parser->process(*QApplication::instance());
    m_aboutData.processCommandLine(m_parser.get());

    const auto args = m_parser->positionalArguments();
    for (const auto &arg : args) {
        addUrl(arg);
    }
}

QString Application::version()
{
    return QString::fromStdString(HARUNA_VERSION_STRING);
}

QList<QUrl> Application::urls()
{
    return m_urls;
}

QUrl Application::url(uint index)
{
    if (m_urls.isEmpty() || index >= m_urls.size()) {
        return {};
    }

    return m_urls.at(index);
}

void Application::addUrl(const QString &value)
{
    m_urls.append(QUrl::fromUserInput(value, QDir::currentPath()));
}

void Application::handleSecondayInstanceMessage(const QByteArray &message, const QString &activationToken)
{
    auto msgString = QString::fromStdString(message.data());
    QFileInfo fileInfo{msgString};
    if (fileInfo.exists() && fileInfo.isFile()) {
        Q_EMIT openUrl(QUrl::fromUserInput(msgString));
        qputenv("XDG_ACTIVATION_TOKEN", activationToken.toUtf8());
        raiseWindow();
    }
}

QStringList Application::availableGuiStyles()
{
    return QStyleFactory::keys();
}

void Application::setGuiStyle(const QString &style)
{
    if (style == u"Default"_s) {
        QApplication::setStyle(m_systemDefaultStyle);
        return;
    }
    QApplication::setStyle(style);
}

QAbstractItemModel *Application::colorSchemesModel()
{
    return m_schemes->model();
}

QQmlApplicationEngine *Application::qmlEngine() const
{
    return m_qmlEngine;
}

void Application::setQmlEngine(QQmlApplicationEngine *_qmlEngine)
{
    m_qmlEngine = _qmlEngine;
}

QCommandLineParser *Application::parser() const
{
    return m_parser.get();
}

void Application::activateColorScheme(const QString &name)
{
    m_schemes->activateScheme(m_schemes->indexForScheme(name));
}

int Application::frameworksVersionMinor()
{
    return KCONFIG_VERSION_MINOR;
}

int Application::qtMajorVersion()
{
    return QT_VERSION_MAJOR;
}

void Application::raiseWindow()
{
    QObject *rootObject = m_qmlEngine->rootObjects().constFirst();
    if (rootObject == nullptr) {
        return;
    }

    QWindow *window = qobject_cast<QWindow *>(rootObject);
    if (window != nullptr) {
        KWindowSystem::updateStartupId(window);
        KWindowSystem::activateWindow(window);
    }
}

#include "moc_application.cpp"
