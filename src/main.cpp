/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "application.h"
#include "generalsettings.h"
#include "kdsingleapplication.h"
#include "qqmlpropertymap.h"
#include "thumbnailimageprovider.h"

#include <QCommandLineParser>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickStyle>

#include <KLocalizedString>

int main(int argc, char *argv[])
{
    qSetMessagePattern(QStringLiteral("%{function}() : %{message}\n"));

    QApplication::setOrganizationName(QStringLiteral("KDE"));
    QApplication::setApplicationName(QStringLiteral("Haruna"));
    QApplication::setOrganizationDomain(QStringLiteral("kde.org"));
    QApplication::setApplicationDisplayName(QStringLiteral("Haruna - Media Player"));
    QApplication::setApplicationVersion(Application::version());

    QQuickStyle::setStyle(QStringLiteral("org.kde.desktop"));
    QQuickStyle::setFallbackStyle(QStringLiteral("Fusion"));
    if (GeneralSettings::useBreezeIconTheme()) {
        QIcon::setThemeName(QStringLiteral("breeze"));
    }

    QApplication qApplication(argc, argv);
    QApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("haruna")));
    KLocalizedString::setApplicationDomain("haruna");

    auto application = Application::instance();

    KDSingleApplication kdsApp;
    if (kdsApp.isPrimaryInstance()) {
        QObject::connect(&kdsApp, &KDSingleApplication::messageReceived, [=](const QByteArray &message) {
            application->handleSecondayInstanceMessage(message);
        });
    } else {
        if (GeneralSettings::self()->useSingleInstance()) {
            QCommandLineParser clParser;
            clParser.process(qApplication);
            if (clParser.positionalArguments().size() > 0) {
                QString file = clParser.positionalArguments().first();
                kdsApp.sendMessage(file.toUtf8());
            }
            exit(EXIT_SUCCESS);
        }
    }

    QQmlApplicationEngine engine(&qApplication);
    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    auto onObjectCreated = [url](const QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl) {
            QCoreApplication::exit(-1);
        }
    };
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &qApplication, onObjectCreated, Qt::QueuedConnection);
    engine.addImportPath(QStringLiteral("qrc:/qml"));
    engine.addImageProvider(QStringLiteral("thumbnail"), new ThumbnailImageProvider());
    engine.rootContext()->setContextProperty(QStringLiteral("app"), Application::instance());
    engine.rootContext()->setContextProperty(QStringLiteral("appActions"), new QQmlPropertyMap);
    engine.rootContext()->setContextObject(new KLocalizedContext(Application::instance()));
    engine.rootContext()->setContextProperty(QStringLiteral("harunaAboutData"), QVariant::fromValue(KAboutData::applicationData()));
    engine.load(url);

    application->setQmlEngine(&engine);

    return qApplication.exec();
}
