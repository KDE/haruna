/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "application.h"

#include "actionsmodel.h"
#include "audiosettings.h"
#include "customcommandsmodel.h"
#include "generalsettings.h"
#include "global.h"
#include "haruna-version.h"
#include "informationsettings.h"
#include "mousesettings.h"
#include "mpvitem.h"
#include "playbacksettings.h"
#include "playlistitem.h"
#include "playlistmodel.h"
#include "playlistsettings.h"
#include "recentfilesmodel.h"
#include "subtitlesfoldersmodel.h"
#include "subtitlessettings.h"
#include "tracksmodel.h"
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
#include <QQuickItem>
#include <QQuickView>
#include <QStandardPaths>
#include <QStyle>
#include <QStyleFactory>
#include <QThread>

#include <KAboutData>
#include <KColorSchemeManager>
#include <KConfig>
#include <KConfigGroup>
#include <KFileItem>
#include <KFileMetaData/Properties>
#include <KLocalizedString>
#include <KStartupInfo>
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
    , m_schemes(new KColorSchemeManager(this))
    , m_systemDefaultStyle(m_app->style()->objectName())
{
    // used to hide playlist when mouse leaves the application
    // while moving between monitors while in fullscreen
    auto *appEventFilter = new ApplicationEventFilter();
    m_app->installEventFilter(appEventFilter);
    QObject::connect(appEventFilter, &ApplicationEventFilter::applicationMouseLeave, this, &Application::qmlApplicationMouseLeave);

    if (GeneralSettings::guiStyle() != QStringLiteral("System")) {
        QApplication::setStyle(GeneralSettings::guiStyle());
    }

    setupWorkerThread();
    setupAboutData();
    setupCommandLineParser();
    registerQmlTypes();
    setupQmlSettingsTypes();

    connect(Global::instance(), &Global::error, this, &Application::error);
    connect(this, &Application::saveWindowGeometryAsync, Worker::instance(), &Worker::saveWindowGeometry, Qt::QueuedConnection);
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
    m_aboutData.setDisplayName(i18nc("application title/display name", "Haruna Media Player"));
    m_aboutData.setVersion(Application::version().toUtf8());
    m_aboutData.setShortDescription(i18nc("@title", "A configurable media player."));
    m_aboutData.setLicense(KAboutLicense::GPL_V3);
    m_aboutData.setCopyrightStatement(i18nc("copyright statement", "(c) 2019-2023"));
    m_aboutData.setHomepage(QStringLiteral("https://haruna.kde.org"));
    m_aboutData.setBugAddress(QStringLiteral("https://bugs.kde.org/enter_bug.cgi?product=Haruna").toUtf8());
    m_aboutData.setDesktopFileName("org.kde.haruna");

    m_aboutData.addAuthor(i18nc("@info:credit", "George Florea Bănuș"),
                          i18nc("@info:credit", "Developer"),
                          QStringLiteral("georgefb899@gmail.com"),
                          QStringLiteral("https://georgefb.com"));

    KAboutData::setApplicationData(m_aboutData);
}

void Application::setupCommandLineParser()
{
    m_parser = new QCommandLineParser();
    m_aboutData.setupCommandLine(m_parser);
    m_parser->addPositionalArgument(QStringLiteral("file"), i18nc("@info:shell", "File to open"));

    QCommandLineOption ytdlFormatSelectionOption(QStringList() << "ytdl-format-selection"
                                                               << "ytdlfs",
                                                 i18nc("@info:shell",
                                                       "Allows to temporarily override the ytdl format selection setting. "
                                                       "Will be overwritten if the setting is changed through the GUI"),
                                                 i18nc("@info:shell", "bestvideo+bestaudio/best"),
                                                 QString());
    m_parser->addOption(ytdlFormatSelectionOption);

    m_parser->process(*m_app);
    m_aboutData.processCommandLine(m_parser);

    for (auto i = 0; i < m_parser->positionalArguments().size(); ++i) {
        addUrl(i, m_parser->positionalArguments().at(i));
    }
}

void Application::registerQmlTypes()
{
    qmlRegisterType<MpvItem>("org.kde.haruna", 1, 0, "MpvItem");
    qRegisterMetaType<PlayListModel *>();
    qRegisterMetaType<PlayListItem *>();
    qRegisterMetaType<TracksModel *>();

#if KCONFIG_VERSION >= QT_VERSION_CHECK(5, 89, 0)
    qRegisterMetaType<KFileMetaData::PropertyMap>("KFileMetaData::PropertyMultiMap");
#else
    qRegisterMetaType<KFileMetaData::PropertyMap>("KFileMetaData::PropertyMap");
#endif
    // models
    qmlRegisterType<SubtitlesFoldersModel>("org.kde.haruna.models", 1, 0, "SubtitlesFoldersModel");
    qmlRegisterType<ActionsModel>("org.kde.haruna.models", 1, 0, "ActionsModel");
    qmlRegisterType<ProxyActionsModel>("org.kde.haruna.models", 1, 0, "ProxyActionsModel");
    qmlRegisterType<CustomCommandsModel>("org.kde.haruna.models", 1, 0, "CustomCommandsModel");
    qmlRegisterType<RecentFilesModel>("org.kde.haruna.models", 1, 0, "RecentFilesModel");
    qmlRegisterType<PlayListProxyModel>("org.kde.haruna.models", 1, 0, "PlayListProxyModel");
}

void Application::setupQmlSettingsTypes()
{
    qmlRegisterSingletonInstance("org.kde.haruna", 1, 0, "AudioSettings", AudioSettings::self());
    qmlRegisterSingletonInstance("org.kde.haruna", 1, 0, "GeneralSettings", GeneralSettings::self());
    qmlRegisterSingletonInstance("org.kde.haruna", 1, 0, "InformationSettings", InformationSettings::self());
    qmlRegisterSingletonInstance("org.kde.haruna", 1, 0, "MouseSettings", MouseSettings::self());
    qmlRegisterSingletonInstance("org.kde.haruna", 1, 0, "PlaybackSettings", PlaybackSettings::self());
    qmlRegisterSingletonInstance("org.kde.haruna", 1, 0, "PlaylistSettings", PlaylistSettings::self());
    qmlRegisterSingletonInstance("org.kde.haruna", 1, 0, "SubtitlesSettings", SubtitlesSettings::self());
    qmlRegisterSingletonInstance("org.kde.haruna", 1, 0, "VideoSettings", VideoSettings::self());
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

QUrl Application::configFilePath(bool withScheme)
{
    auto file = Global::instance()->appConfigFilePath();
    if (file.isEmpty()) {
        return QUrl();
    }
    QUrl url(file);
    if (url.scheme().isEmpty()) {
        url.setScheme("file");
    }
    if (!withScheme) {
        return url.toString(QUrl::PreferLocalFile);
    }
    return url;
}

QUrl Application::ccConfigFilePath(bool withScheme)
{
    auto file = Global::instance()->appConfigFilePath(Global::CustomCommands);
    if (file.isEmpty()) {
        return QUrl();
    }
    QUrl url(file);
    if (url.scheme().isEmpty()) {
        url.setScheme("file");
    }
    if (!withScheme) {
        return url.toString(QUrl::PreferLocalFile);
    }
    return url;
}

QUrl Application::configFolderPath(bool withScheme)
{
    auto folder = Global::instance()->appConfigDirPath();
    if (folder.isEmpty()) {
        return QUrl();
    }
    QUrl url(folder);
    if (url.scheme().isEmpty()) {
        url.setScheme("file");
    }
    if (!withScheme) {
        return url.toString(QUrl::PreferLocalFile);
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
    parentFolderUrl.setScheme("file");

    return parentFolderUrl;
}

QUrl Application::pathToUrl(const QString &path)
{
    QUrl url(path);
    if (!url.isValid()) {
        return QUrl();
    }
    url.setScheme("file");

    return url;
}

bool Application::isYoutubePlaylist(const QString &path)
{
    return path.contains("youtube.com/playlist?list");
}

QString Application::formatTime(const double time)
{
    int totalNumberOfSeconds = static_cast<int>(time);
    int seconds = totalNumberOfSeconds % 60;
    int minutes = (totalNumberOfSeconds / 60) % 60;
    int hours = (totalNumberOfSeconds / 60 / 60);

    QString timeString = QString("%1:%2:%3").arg(hours, 2, 10, QChar('0')).arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0'));

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
    QString content = f.readAll();
    f.close();
    return content;
}

QString Application::mimeType(QUrl url)
{
    KFileItem fileItem(url, KFileItem::NormalMimeTypeDetermination);
    return fileItem.mimetype();
}

QStringList Application::availableGuiStyles()
{
    return QStyleFactory::keys();
}

void Application::setGuiStyle(const QString &style)
{
    if (style == "Default") {
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
    return m_parser;
}

void Application::activateColorScheme(const QString &name)
{
    m_schemes->activateScheme(m_schemes->indexForScheme(name));
}

void Application::openDocs(const QString &page)
{
    QDesktopServices::openUrl(QUrl(page));
}

QStringList Application::getFonts()
{
    static QFontDatabase *fontDB = new QFontDatabase();
    return fontDB->families();
}

int Application::frameworksVersionMinor()
{
    return KCONFIG_VERSION_MINOR;
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
        KStartupInfo::setNewStartupId(window, KStartupInfo::startupId());
        KWindowSystem::activateWindow(window->winId());
    }
}

#include "moc_application.cpp"
