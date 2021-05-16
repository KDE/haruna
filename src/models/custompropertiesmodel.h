#ifndef CUSTOMPROPERTIESMODEL_H
#define CUSTOMPROPERTIESMODEL_H

#include <QAbstractListModel>
#include <KSharedConfig>

class CustomPropertiesModel : public QAbstractListModel
{
    Q_OBJECT

    struct Property {
        QString commandId;
        QString command;
        QString osdMessage;
        QString type;
    };

    enum Roles {
        CommandIdRole = Qt::UserRole + 1,
        CommandRole,
        OsdMessageRole,
        TypeRole,
    };

public:
    explicit CustomPropertiesModel(QObject *parent = nullptr);

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
