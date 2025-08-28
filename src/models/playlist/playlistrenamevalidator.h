/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef PLAYLISTRENAMEVALIDATOR_H
#define PLAYLISTRENAMEVALIDATOR_H

#include <QValidator>

class PlaylistRenameValidator : public QValidator
{
    Q_OBJECT
public:
    explicit PlaylistRenameValidator(QObject *parent = nullptr);
    virtual QValidator::State validate(QString &input, int &pos) const override;
};

#endif // PLAYLISTRENAMEVALIDATOR_H
