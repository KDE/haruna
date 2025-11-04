/*
 * SPDX-FileCopyrightText: 2025 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef MISCUTILS_H
#define MISCUTILS_H

#include <QObject>
#include <QQmlEngine>

class MiscUtils : public QObject
{
    Q_OBJECT
    QML_SINGLETON
    QML_ELEMENT

public:
    static MiscUtils *instance();
    static MiscUtils *create(QQmlEngine *, QJSEngine *);

    Q_INVOKABLE static QString formatTime(const double time);
    Q_INVOKABLE static QString mimeType(QUrl url);

Q_SIGNALS:
    void error(const QString &message);

private:
    MiscUtils();
    ~MiscUtils() = default;

    MiscUtils(const MiscUtils &) = delete;
    MiscUtils &operator=(const MiscUtils &) = delete;
    MiscUtils(MiscUtils &&) = delete;
    MiscUtils &operator=(MiscUtils &&) = delete;
};

#endif // MISCUTILS_H
