#! /usr/bin/env bash
$XGETTEXT `find . \( -name \*.cpp -o -name \*.h -o -name \*.qml \)` -o $podir/haruna.pot
