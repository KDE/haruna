/*
 * SPDX-FileCopyrightText: 2025 Muhammet Sadık Uğursoy <sadikugursoy@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "playlistrenamevalidator.h"

PlaylistRenameValidator::PlaylistRenameValidator(QObject *parent)
    : QValidator{parent}
{
    Q_UNUSED(parent)
}

QValidator::State PlaylistRenameValidator::validate(QString &input, int &pos) const
{
    Q_UNUSED(pos)
    if (input.isEmpty()) {
        return QValidator::Invalid;
    }

    QStringList splitText = input.split(u'.');
    if (splitText.size() == 1) {
        return QValidator::Invalid;
    }

    QString extension = splitText.last();
    if (extension != u"m3u") {
        return QValidator::Invalid;
    }

    return QValidator::Acceptable;
}
