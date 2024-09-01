/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "application.h"

#include "audiosettings.h"
#include "generalsettings.h"
#include "global.h"
#include "haruna-version.h"
#include "informationsettings.h"
#include "mousesettings.h"
#include "mpvitem.h"
#include "mpvpreview.h"
#include "mpvproperties.h"
#include "playbacksettings.h"
#include "playlistsettings.h"
#include "subtitlessettings.h"
#include "videosettings.h"
#include "worker.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QDesktopServices>
#include <QDir>
#include <QFileInfo>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QMimeDatabase>
#include <QQuickView>
#include <QStandardPaths>
#include <QStyle>
#include <QStyleFactory>
#include <QThread>

#include <KAboutData>
#include <KColorSchemeManager>
#include <KConfig>
#include <KConfigGroup>
#include <KCrash>
#include <KFileItem>
#include <KFileMetaData/Properties>
#include <KLocalizedString>
#include <KWindowConfig>
#include <KWindowSystem>

Application *Application::instance()
{
    static Application app;
    return &app;
}

Application::Application()
    : m_app(qApp)
    , m_config(KSharedConfig::openConfig(Global::instance()->appConfigFilePath()))
#if KCOLORSCHEME_VERSION < QT_VERSION_CHECK(6, 6, 0)
    , m_schemes(new KColorSchemeManager(this))
#else
    , m_schemes(KColorSchemeManager::instance())
#endif
    , m_systemDefaultStyle(m_app->style()->objectName())
    , m_appEventFilter{std::make_unique<ApplicationEventFilter>()}
{
    // used to hide playlist when mouse leaves the application
    // while moving between monitors while in fullscreen
    m_app->installEventFilter(m_appEventFilter.get());
    QObject::connect(m_appEventFilter.get(), &ApplicationEventFilter::applicationMouseLeave, this, &Application::qmlApplicationMouseLeave);
    QObject::connect(m_appEventFilter.get(), &ApplicationEventFilter::applicationMouseEnter, this, &Application::qmlApplicationMouseEnter);

    if (GeneralSettings::guiStyle() != QStringLiteral("System")) {
        QApplication::setStyle(GeneralSettings::guiStyle());
    }

    setupWorkerThread();
    setupAboutData();
    setupCommandLineParser();
    setupQmlSettingsTypes();

    KCrash::initialize();

    connect(Global::instance(), &Global::error, this, &Application::error);
    connect(this, &Application::saveWindowGeometryAsync, Worker::instance(), &Worker::saveWindowGeometry, Qt::QueuedConnection);
}

Application::~Application()
{
}

void Application::setupWorkerThread()
{
    auto worker = Worker::instance();
    auto thread = new QThread();
    worker->moveToThread(thread);
    QObject::connect(thread, &QThread::finished, worker, &Worker::deleteLater);
    QObject::connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->start();
}

void Application::setupAboutData()
{
    KAboutData m_aboutData;
    m_aboutData.setComponentName(QStringLiteral("haruna"));
    m_aboutData.setDisplayName(i18nc("application title/display name", "Haruna"));
    m_aboutData.setVersion(Application::version().toUtf8());
    m_aboutData.setShortDescription(i18nc("@title", "Media player"));
    m_aboutData.setLicense(KAboutLicense::GPL_V3);
    m_aboutData.setCopyrightStatement(i18nc("copyright statement", "(c) 2019-2023"));
    m_aboutData.setHomepage(QStringLiteral("https://haruna.kde.org"));
    m_aboutData.setBugAddress(QStringLiteral("https://bugs.kde.org/enter_bug.cgi?product=Haruna").toUtf8());
    m_aboutData.setDesktopFileName(QStringLiteral("org.kde.haruna"));

    m_aboutData.addAuthor(i18nc("@info:credit", "George Florea Bănuș"),
                          i18nc("@info:credit", "Developer"),
                          QStringLiteral("georgefb899@gmail.com"),
                          QStringLiteral("https://georgefb.com"));

    KAboutData::setApplicationData(m_aboutData);
}

void Application::setupCommandLineParser()
{
    m_parser = std::make_unique<QCommandLineParser>();
    m_aboutData.setupCommandLine(m_parser.get());
    m_parser->addPositionalArgument(QStringLiteral("file"), i18nc("@info:shell", "File to open"));

    QCommandLineOption ytdlFormatSelectionOption(QStringList() << QStringLiteral("ytdl-format-selection") << QStringLiteral("ytdlfs"),
                                                 i18nc("@info:shell",
                                                       "Allows to temporarily override the ytdl format selection setting. "
                                                       "Will be overwritten if the setting is changed through the GUI"),
                                                 i18nc("@info:shell", "bestvideo+bestaudio/best"),
                                                 QString());
    m_parser->addOption(ytdlFormatSelectionOption);

    m_parser->process(*m_app);
    m_aboutData.processCommandLine(m_parser.get());

    for (auto i = 0; i < m_parser->positionalArguments().size(); ++i) {
        addUrl(i, m_parser->positionalArguments().at(i));
    }
}

void Application::setupQmlSettingsTypes()
{
    qmlRegisterSingletonInstance("org.kde.haruna.mpvproperties", 1, 0, "MpvProperties", MpvProperties::self());
    qmlRegisterSingletonInstance("org.kde.haruna.settings", 1, 0, "AudioSettings", AudioSettings::self());
    qmlRegisterSingletonInstance("org.kde.haruna.settings", 1, 0, "GeneralSettings", GeneralSettings::self());
    qmlRegisterSingletonInstance("org.kde.haruna.settings", 1, 0, "InformationSettings", InformationSettings::self());
    qmlRegisterSingletonInstance("org.kde.haruna.settings", 1, 0, "MouseSettings", MouseSettings::self());
    qmlRegisterSingletonInstance("org.kde.haruna.settings", 1, 0, "PlaybackSettings", PlaybackSettings::self());
    qmlRegisterSingletonInstance("org.kde.haruna.settings", 1, 0, "PlaylistSettings", PlaylistSettings::self());
    qmlRegisterSingletonInstance("org.kde.haruna.settings", 1, 0, "SubtitlesSettings", SubtitlesSettings::self());
    qmlRegisterSingletonInstance("org.kde.haruna.settings", 1, 0, "VideoSettings", VideoSettings::self());
}

void Application::restoreWindowGeometry(QQuickWindow *window) const
{
    if (!GeneralSettings::rememberWindowGeometry()) {
        return;
    }
    KConfig dataResource(QStringLiteral("data"), KConfig::SimpleConfig, QStandardPaths::AppDataLocation);
    KConfigGroup windowGroup(&dataResource, QStringLiteral("Window"));
    KWindowConfig::restoreWindowSize(window, windowGroup);
    KWindowConfig::restoreWindowPosition(window, windowGroup);
}

void Application::saveWindowGeometry(QQuickWindow *window)
{
    Q_EMIT saveWindowGeometryAsync(window);
}

bool Application::urlExists(const QUrl &url)
{
    return pathExists(url.toLocalFile());
}

bool Application::pathExists(const QString &path)
{
    QFileInfo fileInfo(path);
    if (fileInfo.exists()) {
        return true;
    }
    return false;
}

QUrl Application::configFilePath()
{
    auto file = Global::instance()->appConfigFilePath();
    if (pathExists(file)) {
        return pathToUrl(file);
    }
    return QUrl();
}

QUrl Application::ccConfigFilePath()
{
    auto file = Global::instance()->appConfigFilePath(Global::ConfigFile::CustomCommands);
    if (pathExists(file)) {
        return pathToUrl(file);
    }
    return QUrl();
}

QUrl Application::configFolderPath()
{
    auto folder = Global::instance()->appConfigDirPath();
    if (pathExists(folder)) {
        return pathToUrl(folder);
    }
    return QUrl();
}

QUrl Application::pathToUrl(const QString &path)
{
    auto url = QUrl::fromUserInput(path);
    if (!url.isValid()) {
        return QUrl();
    }
    return url;
}

bool Application::configFolderExists()
{
    QFileInfo fi(Global::instance()->appConfigDirPath());
    return fi.exists();
}

QString Application::version()
{
    return QStringLiteral(HARUNA_VERSION_STRING);
}

bool Application::hasYoutubeDl()
{
    return !youtubeDlExecutable().isEmpty();
}

QString Application::youtubeDlExecutable()
{
    auto ytDlp = QStandardPaths::findExecutable(QStringLiteral("yt-dlp"));
    if (!ytDlp.isEmpty()) {
        return ytDlp;
    }

    auto youtubeDl = QStandardPaths::findExecutable(QStringLiteral("youtube-dl"));
    if (!youtubeDl.isEmpty()) {
        return youtubeDl;
    }

    return QString();
}

QUrl Application::parentUrl(const QString &path)
{
    QUrl url(path);
    if (!url.isValid()) {
        return QUrl(QStandardPaths::writableLocation(QStandardPaths::MoviesLocation));
    }
    QFileInfo fileInfo;
    if (url.isLocalFile()) {
        fileInfo.setFile(url.toLocalFile());
    } else {
        fileInfo.setFile(url.toString());
    }
    QUrl parentFolderUrl(fileInfo.absolutePath());
    parentFolderUrl.setScheme(QStringLiteral("file"));

    return parentFolderUrl;
}

bool Application::isYoutubePlaylist(const QString &path)
{
    return path.contains(QStringLiteral("youtube.com/playlist?list"));
}

QString Application::formatTime(const double time)
{
    int totalNumberOfSeconds = static_cast<int>(time);
    int seconds = totalNumberOfSeconds % 60;
    int minutes = (totalNumberOfSeconds / 60) % 60;
    int hours = (totalNumberOfSeconds / 60 / 60);

    QString hoursString = QStringLiteral("%1").arg(hours, 2, 10, QLatin1Char('0'));
    QString minutesString = QStringLiteral("%1").arg(minutes, 2, 10, QLatin1Char('0'));
    QString secondsString = QStringLiteral("%1").arg(seconds, 2, 10, QLatin1Char('0'));
    QString timeString = QStringLiteral("%1:%2:%3").arg(hoursString, minutesString, secondsString);

    return timeString;
}

QUrl Application::url(int key)
{
    return m_urls[key];
}

void Application::addUrl(int key, const QString &value)
{
    m_urls.insert(key, QUrl::fromUserInput(value, QDir::currentPath()));
}

QString Application::getFileContent(const QString &file)
{
    QFile f(file);
    f.open(QIODevice::ReadOnly);
    QString content = QString::fromUtf8(f.readAll());
    f.close();
    return content;
}

QString Application::mimeType(QUrl url)
{
    KFileItem fileItem(url, KFileItem::NormalMimeTypeDetermination);
    return fileItem.mimetype();
}

void Application::handleSecondayInstanceMessage(const QByteArray &message)
{
    auto msgString = QString::fromStdString(message.data());
    QFileInfo fileInfo{msgString};
    if (fileInfo.exists() && fileInfo.isFile()) {
        Q_EMIT openUrl(QUrl::fromUserInput(msgString));
        raiseWindow();
    }
}

QStringList Application::availableGuiStyles()
{
    return QStyleFactory::keys();
}

void Application::setGuiStyle(const QString &style)
{
    if (style == QStringLiteral("Default")) {
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

QStringList Application::getFonts()
{
    return QFontDatabase::families();
}

int Application::frameworksVersionMinor()
{
    return KCONFIG_VERSION_MINOR;
}

int Application::qtMajorVersion()
{
    return QT_VERSION_MAJOR;
}

QString Application::platformName()
{
    return QGuiApplication::platformName();
}

void Application::raiseWindow()
{
    QObject *m_rootObject = m_qmlEngine->rootObjects().constFirst();
    if (!m_rootObject) {
        return;
    }

    // todo: replace deprecated methods
    QWindow *window = qobject_cast<QWindow *>(m_rootObject);
    if (window) {
        KWindowSystem::activateWindow(window);
    }
}

#include "moc_application.cpp"
