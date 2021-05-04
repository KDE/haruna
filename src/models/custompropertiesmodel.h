#ifndef CUSTOMPROPERTIESMODEL_H
#define CUSTOMPROPERTIESMODEL_H

#include <QAbstractListModel>
#include <KSharedConfig>

class CustomPropertiesModel : public QAbstractListModel
{
    Q_OBJECT

    struct Property {
        QString command;
        QString osdMessage;
        QString shortcut;
        bool setAtStartUp;
    };

    enum Roles {
        CommandRole = Qt::UserRole + 1,
        OsdMessageRole,
        ShortcutRole,
        SetAtStartUp,
    };

public:
    explicit CustomPropertiesModel(QObject *parent = nullptr);

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const;

private:
    KSharedConfig::Ptr m_customPropsConfig;
    QList<Property> m_customProperties;
};

#endif // CUSTOMPROPERTIESMODEL_H
