/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "application.h"
#include "generalsettings.h"
#include "qqmlpropertymap.h"
#include "thumbnailimageprovider.h"

#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickStyle>

#include <KLocalizedString>

int main(int argc, char *argv[])
{
    qSetMessagePattern(QStringLiteral("%{function}() : %{message}\n"));

    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication::setOrganizationName("KDE");
    QApplication::setApplicationName("Haruna");
    QApplication::setOrganizationDomain("kde.org");
    QApplication::setApplicationDisplayName("Haruna - Media Player");
    QApplication::setApplicationVersion(Application::version());

    QQuickStyle::setStyle(QStringLiteral("org.kde.desktop"));
    QQuickStyle::setFallbackStyle(QStringLiteral("Fusion"));
    if (GeneralSettings::useBreezeIconTheme()) {
        QIcon::setThemeName(QStringLiteral("breeze"));
    }

    QApplication qApplication(argc, argv);
    QApplication::setWindowIcon(QIcon::fromTheme("haruna"));
    KLocalizedString::setApplicationDomain("haruna");

    Application::instance();

    QQmlApplicationEngine engine(&qApplication);
    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    auto onObjectCreated = [url](const QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl) {
            QCoreApplication::exit(-1);
        }
    };
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &qApplication, onObjectCreated, Qt::QueuedConnection);
    engine.addImportPath("qrc:/qml");
    engine.addImageProvider("thumbnail", new ThumbnailImageProvider());
    engine.rootContext()->setContextProperty("app", Application::instance());
    engine.rootContext()->setContextProperty("appActions", new QQmlPropertyMap);
    engine.rootContext()->setContextObject(new KLocalizedContext(Application::instance()));
    engine.rootContext()->setContextProperty("harunaAboutData", QVariant::fromValue(KAboutData::applicationData()));
    engine.load(url);

    return qApplication.exec();
}
