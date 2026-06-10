
#include "commandlineoptions.h"

#include <QApplication>
#include <QDir>
#include <QQmlEngine>

#include <KLocalizedString>

using namespace Qt::StringLiterals;

CommandLineOptions &CommandLineOptions::instance()
{
    static CommandLineOptions clo;
    return clo;
}

CommandLineOptions *CommandLineOptions::create(QQmlEngine *, QJSEngine *)
{
    QQmlEngine::setObjectOwnership(&instance(), QQmlEngine::CppOwnership);
    return &instance();
}

CommandLineOptions::CommandLineOptions(QObject *parent)
    : QObject{parent}
    , m_parser(std::make_shared<QCommandLineParser>())
{
    m_parser->addPositionalArgument(u"file"_s, i18nc("@info:shell", "File to open"));

    QCommandLineOption ytdlFormatSelectionOption(QStringList() << u"ytdl-format-selection"_s << u"ytdlfs"_s,
                                                 i18nc("@info:shell",
                                                       "Allows to temporarily override the yt-dlp format selection setting. "
                                                       "Will be overwritten if the setting is changed through the GUI"),
                                                 u"bestvideo+bestaudio/best"_s,
                                                 QString());
    m_parser->addOption(ytdlFormatSelectionOption);

    m_parser->process(*QApplication::instance());
    const auto args = m_parser->positionalArguments();
    for (const auto &arg : args) {
        m_startupUrls.append(QUrl::fromUserInput(arg, QDir::currentPath()));
    }
}

std::shared_ptr<QCommandLineParser> CommandLineOptions::parser() const
{
    return m_parser;
}

QList<QUrl> CommandLineOptions::startupUrls() const
{
    return m_startupUrls;
}

void CommandLineOptions::setStartupUrls(const QList<QUrl> &newStartupUrls)
{
    if (m_startupUrls == newStartupUrls) {
        return;
    }
    m_startupUrls = newStartupUrls;
    Q_EMIT startupUrlsChanged();
}

#include "moc_commandlineoptions.cpp"