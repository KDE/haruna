/*
 * SPDX-FileCopyrightText: 2025 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef SYSTEMUTILS_H
#define SYSTEMUTILS_H

#include <QObject>
#include <QQmlEngine>

class SystemUtils : public QObject
{
    Q_OBJECT
    QML_SINGLETON
    QML_ELEMENT
public:
    explicit SystemUtils(QObject *parent = nullptr);

    Q_INVOKABLE QStringList getFonts();
    Q_INVOKABLE QString platformName();
    Q_INVOKABLE bool isPlatformWayland();
    Q_INVOKABLE static bool isHanaInstalled();
    Q_INVOKABLE static bool openHana(QUrl url);
};

#endif // SYSTEMUTILS_H
