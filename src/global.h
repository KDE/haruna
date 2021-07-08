#ifndef GLOBALS_H
#define GLOBALS_H

#include <QStandardPaths>
#include <KSharedConfig>

class Global
{
public:
    Global();

    enum ConfigFile {
        Main,
        CustomCommands
    };

    const QString systemConfigPath();
    const QString appConfigDirPath();
    const QString appConfigFilePath(ConfigFile configFile = ConfigFile::Main);

    static Global *instance();

private:
    KSharedConfig::Ptr m_config;
    KSharedConfig::Ptr m_ccConfig;
    static Global *sm_instance;
};

#endif // GLOBALS_H
