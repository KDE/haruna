#! /usr/bin/env bash
#
# SPDX-FileCopyrightText: 2021 George Florea Bănuș <georgefb899@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

# ensure it is defined
podir=${podir:?}

$EXTRACTRC `find . -name \*.kcfg` >> rc.cpp
$XGETTEXT $(find . -name \*.cpp -o -name \*.h) -o "$podir"/haruna.pot
# Extract JavaScripty files as what they are, otherwise for example
# template literals won't work correctly (by default we extract as C++).
# https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Template_literals
$XGETTEXT --join-existing --language=JavaScript $(find . -name \*.qml -o -name \*.js) -o "$podir"/haruna.pot

