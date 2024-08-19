/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef TRACKSMODEL_H
#define TRACKSMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <QtQml/qqmlregistration.h>

class TracksModel : public QAbstractListModel
{
    Q_OBJECT
    QML_NAMED_ELEMENT(TracksModel)

public:
    explicit TracksModel(QObject *parent = nullptr);
    enum { TextRole = Qt::UserRole, LanguageRole, TitleRole, IDRole, CodecRole };
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

public Q_SLOTS:
    void setTracks(QList<QVariant> tracks);

private:
    QList<QVariant> m_tracks;
};

#endif // TRACKSMODEL_H
