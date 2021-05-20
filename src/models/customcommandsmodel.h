#ifndef CUSTOMCOMMANDSMODEL_H
#define CUSTOMCOMMANDSMODEL_H

#include <QAbstractListModel>
#include <QSortFilterProxyModel>
#include <KSharedConfig>

class ProxyCustomCommandsModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit ProxyCustomCommandsModel(QObject *parent = nullptr);
};

class CustomCommandsModel : public QAbstractListModel
{
    Q_OBJECT

    struct Command {
        QString commandId;
        QString command;
        QString osdMessage;
        QString type;
        int order;
    };

public:
    explicit CustomCommandsModel(QObject *parent = nullptr);

    enum Roles {
        CommandIdRole = Qt::UserRole + 1,
        CommandRole,
        OsdMessageRole,
        TypeRole,
    };

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const;

    Q_INVOKABLE void moveRows(int oldIndex, int newIndex);
    Q_INVOKABLE void saveCustomCommand(const QString &command,
                                       const QString &osdMessage,
                                       const QString &type);
    Q_INVOKABLE void editCustomCommand(int row,
                                       const QString &command,
                                       const QString &osdMessage,
                                       const QString &type);
    Q_INVOKABLE void deleteCustomCommand(const QString &groupName, int row);

private:
    KSharedConfig::Ptr m_customCommandsConfig;
    QList<Command *> m_customCommands;
};

#endif // CUSTOMCOMMANDSMODEL_H
