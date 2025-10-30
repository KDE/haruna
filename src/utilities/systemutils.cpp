#include "systemutils.h"

#include <QFontDatabase>
#include <QGuiApplication>

#include <KWindowSystem>

SystemUtils::SystemUtils(QObject *parent)
    : QObject{parent}
{

}

QStringList SystemUtils::getFonts()
{
    return QFontDatabase::families();
}

QString SystemUtils::platformName()
{
    return QGuiApplication::platformName();
}

bool SystemUtils::isPlatformWayland()
{
    return KWindowSystem::isPlatformWayland();
}
