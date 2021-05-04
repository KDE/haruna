/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef SUBTITLESFOLDERSMODEL_H
#define SUBTITLESFOLDERSMODEL_H

#include <QAbstractListModel>
#include <KSharedConfig>

class SubtitlesFoldersModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit SubtitlesFoldersModel(QObject *parent = nullptr);

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

public slots:
    void updateFolder(const QString &folder, int row);
    void deleteFolder(int row);
    void addFolder();

private:
    QStringList m_list;
    KSharedConfig::Ptr m_config;
};

#endif // SUBTITLESFOLDERSMODEL_H
