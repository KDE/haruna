/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "application.h"

int main(int argc, char *argv[])
{
    qSetMessagePattern(QStringLiteral("%{function}() : %{message}\n"));
    Application app(argc, argv, "Haruna");

    return app.run();
}

