/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef TRACKSMODEL_H
#define TRACKSMODEL_H

#include <QAbstractListModel>
#include <QtQml/qqmlregistration.h>

struct Track {
    int trackid{-1};
    QString lang;
    QString title;
    QString codec;
};

class TracksModel : public QAbstractListModel
{
    Q_OBJECT
    QML_NAMED_ELEMENT(TracksModel)

public:
    explicit TracksModel(QObject *parent = nullptr);

    // clang-format off
    enum Roles {
        IdRole = Qt::UserRole,
        TextRole,
        LanguageRole,
        TitleRole,
        CodecRole
    };
    Q_ENUM(Roles)
    // clang-format on

    Q_PROPERTY(int rowCount READ rowCount NOTIFY rowCountChanged)
    Q_SIGNAL void rowCountChanged();
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void clear();
    void addTrack(Track track);

public Q_SLOTS:
    void setTracks(QList<Track> tracks);

private:
    QList<Track> m_data;
};

#endif // TRACKSMODEL_H
