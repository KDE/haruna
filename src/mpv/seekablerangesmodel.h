/*
 * SPDX-FileCopyrightText: 2026 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef SEEKABLERANGESMODEL_H
#define SEEKABLERANGESMODEL_H

#include <QAbstractListModel>

struct SeekableRange {
    double start = 0.0;
    double end = 0.0;
};

class SeekableRangesModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit SeekableRangesModel(QObject *parent = nullptr);

    enum class Roles : int {
        StartRole = Qt::UserRole,
        EndRole,
    };
    Q_ENUM(Roles)

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    void populate(const QList<QVariant> data);

private:
    QList<SeekableRange> m_data;
};

#endif // SEEKABLERANGESMODEL_H
