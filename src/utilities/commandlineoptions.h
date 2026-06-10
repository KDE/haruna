#ifndef COMMANDLINEOPTIONS_H
#define COMMANDLINEOPTIONS_H

#include <QCommandLineParser>
#include <QObject>
#include <QUrl>
#include <qqmlintegration.h>

class QJSEngine;
class QQmlEngine;

class CommandLineOptions : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    static CommandLineOptions *instance();
    static CommandLineOptions *create(QQmlEngine *, QJSEngine *);

    Q_PROPERTY(QList<QUrl> startupUrls READ startupUrls WRITE setStartupUrls NOTIFY startupUrlsChanged FINAL)
    Q_SIGNAL void startupUrlsChanged();
    QList<QUrl> startupUrls() const;
    void setStartupUrls(const QList<QUrl> &newStartupUrls);

    std::shared_ptr<QCommandLineParser> parser() const;

private:
    explicit CommandLineOptions(QObject *parent = nullptr);
    QList<QUrl> m_startupUrls;
    std::shared_ptr<QCommandLineParser> m_parser;
};

#endif // COMMANDLINEOPTIONS_H
