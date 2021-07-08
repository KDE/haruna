#include "global.h"

#include "_debug.h"

#include <QFileInfo>

Global* Global::sm_instance = nullptr;

Global *Global::instance()
{
    if (!sm_instance) {
        sm_instance = new Global();
    }
    return sm_instance;
}

Global::Global()
{
    m_config = KSharedConfig::openConfig("haruna/haruna.conf");
    m_ccConfig = KSharedConfig::openConfig("haruna/haruna-custom-commands.conf");
}

const QString Global::systemConfigPath()
{
    return QStandardPaths::writableLocation(m_config->locationType()).append("/");
}

const QString Global::appConfigDirPath()
{
    QFileInfo configFile(QString(systemConfigPath()).append(m_config->name()));
    return configFile.absolutePath();
}

const QString Global::appConfigFilePath(ConfigFile configFile)
{
    switch (configFile) {
    case ConfigFile::Main:
        return QString(systemConfigPath()).append(m_config->name());
    case ConfigFile::CustomCommands:
        return QString(systemConfigPath()).append(m_ccConfig->name());
    }
}

