/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QCommandLineParser>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickStyle>
#include <QQuickWindow>

#include <KLocalizedString>

#include "application.h"
#include "generalsettings.h"
#include "kdsingleapplication.h"
#include "qqmlpropertymap.h"
#include "thumbnailimageprovider.h"

using namespace Qt::StringLiterals;

int main(int argc, char *argv[])
{
    qSetMessagePattern(u"%{function}() : %{message}\n"_s);

    QApplication::setOrganizationName(u"KDE"_s);
    QApplication::setApplicationName(u"Haruna"_s);
    QApplication::setOrganizationDomain(u"kde.org"_s);
    QApplication::setApplicationDisplayName(u"Haruna - Media Player"_s);
    QApplication::setApplicationVersion(Application::version());

    // required by mpv
    QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);

    QQuickStyle::setStyle(u"org.kde.desktop"_s);
    QQuickStyle::setFallbackStyle(u"Fusion"_s);
    if (GeneralSettings::useBreezeIconTheme()) {
        QIcon::setThemeName(u"breeze"_s);
    }

    QApplication qApplication(argc, argv);
    QApplication::setWindowIcon(QIcon::fromTheme(u"haruna"_s));
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
                QString file = clParser.positionalArguments().constFirst();
                kdsApp.sendMessage(file.toUtf8());
            }
            exit(EXIT_SUCCESS);
        }
    }

    QQmlApplicationEngine engine(&qApplication);
    engine.addImageProvider(u"thumbnail"_s, new ThumbnailImageProvider());
    engine.rootContext()->setContextProperty(u"app"_s, Application::instance());
    engine.rootContext()->setContextProperty(u"appActions"_s, new QQmlPropertyMap);
    engine.rootContext()->setContextObject(new KLocalizedContext(Application::instance()));
    engine.loadFromModule("org.kde.haruna", "Main");

    application->setQmlEngine(&engine);

    return qApplication.exec();
}
