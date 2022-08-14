/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "application.h"

#include "audiosettings.h"
#include "customcommandsmodel.h"
#include "generalsettings.h"
#include "global.h"
#include "haruna-version.h"
#include "lockmanager.h"
#include "mediaplayer2.h"
#include "mediaplayer2player.h"
#include "mousesettings.h"
#include "mpvitem.h"
#include "playbacksettings.h"
#include "playlistitem.h"
#include "playlistmodel.h"
#include "playlistsettings.h"
#include "subtitlesfoldersmodel.h"
#include "subtitlessettings.h"
#include "thumbnailimageprovider.h"
#include "tracksmodel.h"
#include "videosettings.h"
#include "worker.h"

#include <clocale>

#include <QApplication>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDesktopServices>
#include <QDir>
#include <QDBusConnection>
#include <QFileInfo>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QMimeDatabase>
#include <QPointer>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickStyle>
#include <QQuickView>
#include <QStandardPaths>
#include <QStyle>
#include <QStyleFactory>
#include <QThread>
#include <KFileItem>
#include <recentfilesmodel.h>
#include <actionsmodel.h>

#include <KAboutApplicationDialog>
#include <KAboutData>
#include <KColorSchemeManager>
#include <KConfig>
#include <KConfigGroup>
#include <KFileMetaData/Properties>
#include <KLocalizedContext>
#include <KLocalizedString>
#include <KWindowConfig>

static QApplication *createApplication(int &argc, char **argv, const QString &applicationName)
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication::setOrganizationName("KDE");
    QApplication::setApplicationName(applicationName);
    QApplication::setOrganizationDomain("kde.org");
    QApplication::setApplicationDisplayName("Haruna - Video Player");
    QApplication::setApplicationVersion(Application::version());

    QQuickStyle::setStyle(QStringLiteral("org.kde.desktop"));
    QQuickStyle::setFallbackStyle(QStringLiteral("Fusion"));
    if (GeneralSettings::useBreezeIconTheme()) {
        QIcon::setThemeName(QStringLiteral("breeze"));
    }

    QApplication *app = new QApplication(argc, argv);
    QApplication::setWindowIcon(QIcon::fromTheme("haruna"));
    KLocalizedString::setApplicationDomain("haruna");
    return app;
}

Application::Application(int &argc, char **argv, const QString &applicationName)
    : m_app(createApplication(argc, argv, applicationName))
{
    m_config = KSharedConfig::openConfig(Global::instance()->appConfigFilePath());
    m_schemes = new KColorSchemeManager(this);
    m_systemDefaultStyle = m_app->style()->objectName();

    // used to hide playlist when mouse leaves the application
    // while moving between monitors while in fullscreen
    auto *appEventFilter = new ApplicationEventFilter();
    m_app->installEventFilter(appEventFilter);
    QObject::connect(appEventFilter, &ApplicationEventFilter::applicationMouseLeave,
                     this, &Application::qmlApplicationMouseLeave);

    // register mpris dbus service
    QString mspris2Name(QStringLiteral("org.mpris.MediaPlayer2.haruna"));
    QDBusConnection::sessionBus().registerService(mspris2Name);
    QDBusConnection::sessionBus().registerObject(QStringLiteral("/org/mpris/MediaPlayer2"),
                                                 this, QDBusConnection::ExportAdaptors);
    // org.mpris.MediaPlayer2 mpris2 interface
    new MediaPlayer2(this);

    if (GeneralSettings::guiStyle() != QStringLiteral("System")) {
        QApplication::setStyle(GeneralSettings::guiStyle());
    }

    // Qt sets the locale in the QGuiApplication constructor, but libmpv
    // requires the LC_NUMERIC category to be set to "C", so change it back.
    std::setlocale(LC_NUMERIC, "C");

    setupWorkerThread();
    setupAboutData();
    setupCommandLineParser();
    registerQmlTypes();
    setupQmlSettingsTypes();

    m_engine = new QQmlApplicationEngine(this);
    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    auto onObjectCreated = [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl) {
            QCoreApplication::exit(-1);
        }
    };
    QObject::connect(m_engine, &QQmlApplicationEngine::objectCreated,
                     m_app, onObjectCreated, Qt::QueuedConnection);
    m_engine->addImportPath("qrc:/qml");
    m_engine->addImageProvider("thumbnail", new ThumbnailImageProvider());
    setupQmlContextProperties();
    m_engine->load(url);

    connect(Global::instance(), &Global::error, this, &Application::error);
}

Application::~Application()
{
    delete m_engine;
}

int Application::run()
{
    return m_app->exec();
}

void Application::setupWorkerThread()
{
    auto worker = Worker::instance();
    auto thread = new QThread();
    worker->moveToThread(thread);
    QObject::connect(thread, &QThread::finished,
                     worker, &Worker::deleteLater);
    QObject::connect(thread, &QThread::finished,
                     thread, &QThread::deleteLater);
    thread->start();
}

void Application::setupAboutData()
{
    m_aboutData = KAboutData(QStringLiteral("haruna"),
                             i18n("Haruna Video Player"),
                             Application::version());
    m_aboutData.setShortDescription(i18n("A configurable video player."));
    m_aboutData.setLicense(KAboutLicense::GPL_V3);
    m_aboutData.setCopyrightStatement(i18n("(c) 2019-2021"));
    m_aboutData.setHomepage(QStringLiteral("https://invent.kde.org/multimedia/haruna"));
    m_aboutData.setBugAddress(QStringLiteral("https://bugs.kde.org/enter_bug.cgi?product=Haruna").toUtf8());
    m_aboutData.setComponentName(QStringLiteral("haruna"));
    m_aboutData.setDesktopFileName("org.kde.haruna");

    m_aboutData.addAuthor(i18n("George Florea Bănuș"),
                        i18n("Developer"),
                        QStringLiteral("georgefb899@gmail.com"),
                        QStringLiteral("https://georgefb.com"));

    KAboutData::setApplicationData(m_aboutData);
}

void Application::setupCommandLineParser()
{
    QCommandLineParser parser;
    m_aboutData.setupCommandLine(&parser);
    parser.addPositionalArgument(QStringLiteral("file"), i18n("File to open"));
    parser.process(*m_app);
    m_aboutData.processCommandLine(&parser);

    for (auto i = 0; i < parser.positionalArguments().size(); ++i) {
        addArgument(i, parser.positionalArguments().at(i));
    }
}

void Application::registerQmlTypes()
{
    qmlRegisterType<MpvItem>("org.kde.haruna", 1, 0, "MpvItem");
    qRegisterMetaType<PlayListModel*>();
    qRegisterMetaType<PlayListItem*>();
    qRegisterMetaType<TracksModel*>();
    qRegisterMetaType<KFileMetaData::PropertyMap>("KFileMetaData::PropertyMap");
}

void Application::setupQmlSettingsTypes()
{
    auto audioProvider = [](QQmlEngine *, QJSEngine *) -> QObject * { return AudioSettings::self(); };
    qmlRegisterSingletonType<AudioSettings>("org.kde.haruna", 1, 0, "AudioSettings", audioProvider);

    auto generalProvider = [](QQmlEngine *, QJSEngine *) -> QObject * { return GeneralSettings::self(); };
    qmlRegisterSingletonType<GeneralSettings>("org.kde.haruna", 1, 0, "GeneralSettings", generalProvider);

    auto mouseProvider = [](QQmlEngine *, QJSEngine *) -> QObject * { return MouseSettings::self(); };
    qmlRegisterSingletonType<MouseSettings>("org.kde.haruna", 1, 0, "MouseSettings", mouseProvider);

    auto playbackProvider = [](QQmlEngine *, QJSEngine *) -> QObject * { return PlaybackSettings::self(); };
    qmlRegisterSingletonType<PlaybackSettings>("org.kde.haruna", 1, 0, "PlaybackSettings", playbackProvider);

    auto playlistProvider = [](QQmlEngine *, QJSEngine *) -> QObject * { return PlaylistSettings::self(); };
    qmlRegisterSingletonType<PlaylistSettings>("org.kde.haruna", 1, 0, "PlaylistSettings", playlistProvider);

    auto subtitlesProvider = [](QQmlEngine *, QJSEngine *) -> QObject * { return SubtitlesSettings::self(); };
    qmlRegisterSingletonType<SubtitlesSettings>("org.kde.haruna", 1, 0, "SubtitlesSettings", subtitlesProvider);

    auto videoProvider = [](QQmlEngine *, QJSEngine *) -> QObject * { return VideoSettings::self(); };
    qmlRegisterSingletonType<VideoSettings>("org.kde.haruna", 1, 0, "VideoSettings", videoProvider);
}

void Application::setupQmlContextProperties()
{
    std::unique_ptr<LockManager> lockManager = std::make_unique<LockManager>();
    std::unique_ptr<SubtitlesFoldersModel> subsFoldersModel = std::make_unique<SubtitlesFoldersModel>();

    m_engine->rootContext()->setContextProperty(QStringLiteral("app"), this);
    qmlRegisterUncreatableType<Application>("Application", 1, 0, "Application",
                                            QStringLiteral("Application should not be created in QML"));

    m_engine->rootContext()->setContextProperty(QStringLiteral("mediaPlayer2Player"), new MediaPlayer2Player(this));

    m_engine->rootContext()->setContextProperty(QStringLiteral("lockManager"), lockManager.release());
    qmlRegisterUncreatableType<LockManager>("LockManager", 1, 0, "LockManager",
                                            QStringLiteral("LockManager should not be created in QML"));

    m_engine->rootContext()->setContextProperty(QStringLiteral("subsFoldersModel"), subsFoldersModel.release());


    auto actionsModel = new ActionsModel();
    m_engine->rootContext()->setContextProperty(QStringLiteral("actionsModel"), actionsModel);
    m_engine->rootContext()->setContextProperty(QStringLiteral("appActions"), &actionsModel->propertyMap);
    auto proxyActionsModel = new ProxyActionsModel();
    proxyActionsModel->setSourceModel(actionsModel);
    m_engine->rootContext()->setContextProperty(QStringLiteral("proxyActionsModel"), proxyActionsModel);

    auto customCommandsModel = new CustomCommandsModel(actionsModel);
    m_engine->rootContext()->setContextProperty(QStringLiteral("customCommandsModel"), customCommandsModel);

    auto recentFilesModel = new RecentFilesModel();
    m_engine->rootContext()->setContextProperty(QStringLiteral("recentFilesModel"), recentFilesModel);

    m_engine->rootContext()->setContextObject(new KLocalizedContext(this));
    m_engine->rootContext()->setContextProperty(QStringLiteral("harunaAboutData"),
                                                QVariant::fromValue(KAboutData::applicationData()));
}

void Application::restoreWindowGeometry(QQuickWindow *window) const
{
    if(!GeneralSettings::rememberWindowGeometry()) {
        return;
    }
    KConfig dataResource(QStringLiteral("data"), KConfig::SimpleConfig, QStandardPaths::AppDataLocation);
    KConfigGroup windowGroup(&dataResource, QStringLiteral("Window"));
    KWindowConfig::restoreWindowSize(window, windowGroup);
    KWindowConfig::restoreWindowPosition(window, windowGroup);
}

void Application::saveWindowGeometry(QQuickWindow *window) const
{
    if(!GeneralSettings::rememberWindowGeometry()) {
        return;
    }
    KConfig dataResource(QStringLiteral("data"), KConfig::SimpleConfig, QStandardPaths::AppDataLocation);
    KConfigGroup windowGroup(&dataResource, QStringLiteral("Window"));
    KWindowConfig::saveWindowPosition(window, windowGroup);
    KWindowConfig::saveWindowSize(window, windowGroup);
    dataResource.sync();
}

QUrl Application::configFilePath()
{
    QUrl url(Global::instance()->appConfigFilePath());
    url.setScheme("file");
    return url;
}

QUrl Application::configFolderPath()
{
    QUrl url(Global::instance()->appConfigDirPath());
    url.setScheme("file");
    return url;
}

bool Application::configFolderExists()
{
    QFileInfo fi(Global::instance()->appConfigDirPath());
    return fi.exists();
}

QString Application::getDefaultFontColor()
{
    return SubtitlesSettings::self()->defaultFontColorValue();
}

QString Application::getDefaultSubShadowColor()
{
    return SubtitlesSettings::self()->defaultShadowColorValue();
}

QString Application::getDefaultSubBorderColor()
{
    return SubtitlesSettings::self()->defaultBorderColorValue();
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
        return QStandardPaths::writableLocation(QStandardPaths::MoviesLocation);
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
    QTime t(0, 0, 0);
    QString formattedTime = t.addSecs(static_cast<qint64>(time)).toString("hh:mm:ss");
    return formattedTime;
}

QString Application::argument(int key)
{
    return m_args[key];
}

void Application::addArgument(int key, const QString &value)
{
    m_args.insert(key, value);
}

QString Application::getFileContent(const QString &file)
{
    QFile f(file);
    f.open(QIODevice::ReadOnly);
    QString content = f.readAll();
    f.close();
    return content;
}

QString Application::mimeType(const QString &file)
{
    QMimeDatabase db;
    QMimeType mimeType;
    if(KFileItem(file).isSlow()) {
        mimeType = db.mimeTypeForFile(file, QMimeDatabase::MatchExtension);
    } else {
        mimeType = db.mimeTypeForFile(file);
    }
    return mimeType.name();
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

void Application::activateColorScheme(const QString &name)
{
    m_schemes->activateScheme(m_schemes->indexForScheme(name));
}

void Application::aboutApplication()
{
    static QPointer<QDialog> dialog;
    if (!dialog) {
        dialog = new KAboutApplicationDialog(KAboutData::applicationData(), nullptr);
        dialog->setAttribute(Qt::WA_DeleteOnClose);
    }
    dialog->show();
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
