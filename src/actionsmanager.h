/*
 * SPDX-FileCopyrightText: 2021 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef ACTIONS_H
#define ACTIONS_H

#include <QObject>
#include <KActionCollection>
#include <KSharedConfig>

class KConfigDialog;

class ActionsManager : public QObject
{
    Q_OBJECT
public:
    explicit ActionsManager(QObject *parent = nullptr);

    Q_INVOKABLE QAction *action(const QString &name);
    Q_INVOKABLE void configureShortcuts(const QString &name = QString());
    Q_INVOKABLE void createUserAction(const QString &text);

private:
    void setupActions(const QString &actionName);
    void setupUserActions();

    KActionCollection m_collection;
    KConfigGroup *m_shortcuts;
};

#endif // ACTIONS_H
