/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef TRACKSMODEL_H
#define TRACKSMODEL_H

#include <QAbstractListModel>
#include <QtQml/qqmlregistration.h>

using namespace Qt::StringLiterals;

struct Track {
    int trackid{-1};
    QString lang;
    QString title;
    QString codec;

    QString display() const
    {
        QString text;
        auto _title = title;
        if (!_title.isEmpty()) {
            text += _title.append(u" "_s);
        }
        auto _lang = lang;
        if (!_lang.isEmpty()) {
            text += _lang.append(u" "_s);
        }
        if (!codec.isEmpty()) {
            text += codec;
        }

        return text;
    }
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
    const Track track(int row) const;
    QString trackInfo(int row) const;

    int activeRow() const;
    void setActiveRow(int newActiveTrack);
    int nextRow();
    int previousRow();

public Q_SLOTS:
    void setTracks(QList<Track> tracks);

private:
    QList<Track> m_data;
    int m_activeRow{-1};
};

#endif // TRACKSMODEL_H
