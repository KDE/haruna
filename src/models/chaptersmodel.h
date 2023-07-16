#ifndef CHAPTERSMODEL_H
#define CHAPTERSMODEL_H

#include <QAbstractListModel>

struct Chapter {
    QString title;
    double startTime;
};

class ChaptersModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit ChaptersModel(QObject *parent = nullptr);

    enum Roles {
        TitleRole = Qt::UserRole + 1,
        StartTimeRole,
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual QHash<int, QByteArray> roleNames() const override;

    void setChapters(QList<Chapter> &_chapters);

private:
    QList<Chapter> m_chapters;
};

#endif // CHAPTERSMODEL_H
