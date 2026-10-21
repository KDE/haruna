#include "fontsmodel.h"

FontsModel::FontsModel(QObject *parent)
    : QStringListModel(parent)
{
}

QVariant FontsModel::data(const QModelIndex &index, int role) const
{
    return QStringListModel::data(index, role);
}

QHash<int, QByteArray> FontsModel::roleNames() const
{
    auto roles = QStringListModel::roleNames();
    roles[static_cast<int>(Roles::FontNameRole)] = "fontName";
    return roles;
}

void FontsModel::setStringList(const QStringList &strings)
{
    QStringListModel::setStringList(strings);
};