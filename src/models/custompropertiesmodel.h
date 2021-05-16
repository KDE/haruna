#ifndef CUSTOMPROPERTIESMODEL_H
#define CUSTOMPROPERTIESMODEL_H

#include <QAbstractListModel>
#include <QSortFilterProxyModel>
#include <KSharedConfig>

class ProxyCustomPropertiesModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit ProxyCustomPropertiesModel(QObject *parent = nullptr);
};

class CustomPropertiesModel : public QAbstractListModel
{
    Q_OBJECT

    struct Property {
        QString commandId;
        QString command;
        QString osdMessage;
        QString type;
    };

public:
    explicit CustomPropertiesModel(QObject *parent = nullptr);

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

    Q_INVOKABLE void getProperties();
    Q_INVOKABLE void moveRows(int oldIndex, int newIndex);
    Q_INVOKABLE void saveCustomProperty(const QString &groupName,
                                        const QString &command,
                                        const QString &osdMessage,
                                        const QString &type);

private:
    KSharedConfig::Ptr m_customPropsConfig;
    QList<Property> m_customProperties;
};

#endif // CUSTOMPROPERTIESMODEL_H
