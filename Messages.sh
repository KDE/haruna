#! /usr/bin/env bash
#
# SPDX-FileCopyrightText: 2021 George Florea Bănuș <georgefb899@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

$EXTRACTRC `find . -name \*.kcfg` >> rc.cpp
$XGETTEXT `find . \( -name \*.cpp -o -name \*.h -o -name \*.qml \)` -o $podir/haruna.pot
