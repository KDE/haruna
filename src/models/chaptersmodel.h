/*
 * SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CHAPTERSMODEL_H
#define CHAPTERSMODEL_H

#include <QAbstractListModel>
#include <QtQml/qqmlregistration.h>

struct Chapter {
    QString title;
    double startTime;
};

class ChaptersModel : public QAbstractListModel
{
    Q_OBJECT
    QML_NAMED_ELEMENT(ChaptersModel)

public:
    explicit ChaptersModel(QObject *parent = nullptr);

    enum Roles {
        TitleRole = Qt::UserRole + 1,
        StartTimeRole,
    };

    Q_PROPERTY(int rowCount READ rowCount NOTIFY rowCountChanged)
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setChapters(QList<Chapter> &_chapters);

Q_SIGNALS:
    void rowCountChanged();

private:
    QList<Chapter> m_chapters;
};

#endif // CHAPTERSMODEL_H
