/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "application.h"
#include "lockmanager.h"
#include "mpvobject.h"
#include "playlist/playlistmodel.h"
#include "settings.h"
#include "subtitlesfoldersmodel.h"
#include "tracksmodel.h"
#include "worker.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickItem>
#include <QQuickStyle>
#include <QQuickView>
#include <QThread>

#include <KAboutData>
#include <KLocalizedString>

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QQuickStyle::setFallbackStyle(QStringLiteral("org.kde.desktop"));

    QApplication app(argc, argv);
    app.setOrganizationName("georgefb");
    app.setOrganizationDomain("georgefb.com");
    app.setWindowIcon(QIcon::fromTheme("com.georgefb.haruna"));

    KAboutData aboutData(
                QStringLiteral("haruna"),
                i18n("Haruna Video Player"),
                QStringLiteral("0.1.0"),
                i18n("A simple video player."),
                KAboutLicense::GPL_V3,
                i18n("(c) 2019"),
                i18n("TO DO..."),
                QStringLiteral("http://georgefb.com/haruna"),
                QStringLiteral("georgefb899@gmail.com"));

    aboutData.addAuthor(i18n("George Florea Bănuș"),
                        i18n("Developer"),
                        QStringLiteral("georgefb899@gmail.com"),
                        QStringLiteral("http://georgefb.com"));

    KAboutData::setApplicationData(aboutData);

    QCommandLineParser parser;
    aboutData.setupCommandLine(&parser);
    parser.addPositionalArgument(QStringLiteral("file"), i18n("Document to open"));
    parser.process(app);
    aboutData.processCommandLine(&parser);

    // Qt sets the locale in the QGuiApplication constructor, but libmpv
    // requires the LC_NUMERIC category to be set to "C", so change it back.
    std::setlocale(LC_NUMERIC, "C");

    qmlRegisterType<MpvObject>("mpv", 1, 0, "MpvObject");
    qRegisterMetaType<QAction>("QAction");
    qRegisterMetaType<TracksModel>("TracksModel");

    std::unique_ptr<Application> myApp = std::make_unique<Application>();
    std::unique_ptr<LockManager> lockManager = std::make_unique<LockManager>();
    std::unique_ptr<SubtitlesFoldersModel> subsFoldersModel = std::make_unique<SubtitlesFoldersModel>();
    std::unique_ptr<Settings> settings = std::make_unique<Settings>();

    for (auto i = 0; i < parser.positionalArguments().size(); ++i) {
        myApp->addArgument(i, parser.positionalArguments().at(i));
    }

    auto worker = Worker::instance();
    auto thread = new QThread();
    worker->moveToThread(thread);
    QObject::connect(thread, &QThread::finished,
                     worker, &Worker::deleteLater);
    QObject::connect(thread, &QThread::finished,
                     thread, &QThread::deleteLater);
    thread->start();

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl) {
            QCoreApplication::exit(-1);
        }
    }, Qt::QueuedConnection);

    PlayListModel playListModel;
    engine.rootContext()->setContextProperty("playListModel", &playListModel);
    qmlRegisterUncreatableType<PlayListModel>("PlayListModel", 1, 0, "PlayListModel",
                                               QStringLiteral("PlayListModel should not be created in QML"));

    engine.rootContext()->setContextProperty(QStringLiteral("app"), myApp.release());
    qmlRegisterUncreatableType<Application>("Application", 1, 0, "Application",
                                            QStringLiteral("Application should not be created in QML"));

    engine.rootContext()->setContextProperty(QStringLiteral("lockManager"), lockManager.release());
    qmlRegisterUncreatableType<LockManager>("LockManager", 1, 0, "LockManager",
                                            QStringLiteral("LockManager should not be created in QML"));

    engine.rootContext()->setContextProperty(QStringLiteral("subsFoldersModel"), subsFoldersModel.release());
    engine.rootContext()->setContextProperty(QStringLiteral("settings"), settings.release());
    engine.load(url);
    return app.exec();
}

