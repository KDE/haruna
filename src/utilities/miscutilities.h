#ifndef MISCUTILITIES_H
#define MISCUTILITIES_H

#include <QObject>
#include <QQmlEngine>

class MiscUtilities : public QObject
{
    Q_OBJECT
    QML_SINGLETON
    QML_ELEMENT

public:
    explicit MiscUtilities(QObject *parent = nullptr);

    Q_INVOKABLE static QString formatTime(const double time);
    Q_INVOKABLE static QString mimeType(QUrl url);
};

#endif // MISCUTILITIES_H
