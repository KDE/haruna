/*
 * SPDX-FileCopyrightText: 2025 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef MISCUTILITIES_H
#define MISCUTILITIES_H

#include <QObject>
#include <QQmlEngine>

class MiscUtils : public QObject
{
    Q_OBJECT
    QML_SINGLETON
    QML_ELEMENT

public:
    explicit MiscUtils(QObject *parent = nullptr);

    Q_INVOKABLE static QString formatTime(const double time);
    Q_INVOKABLE static QString mimeType(QUrl url);
};

#endif // MISCUTILITIES_H
