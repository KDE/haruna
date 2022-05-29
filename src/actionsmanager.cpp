/*
 * SPDX-FileCopyrightText: 2021 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "actionsmanager.h"

#include "global.h"
#include "haction.h"

#include <KConfigGroup>
#include <KLocalizedString>
#include <KShortcutsDialog>
#include <KTreeWidgetSearchLine>

#include <QApplication>

ActionsManager::ActionsManager(QObject *parent)
    : QObject(parent)
    , m_collection(this)
{
    auto config = KSharedConfig::openConfig(Global::instance()->appConfigFilePath());
    m_shortcuts = new KConfigGroup(config, "Shortcuts");

    setupUserActions();
}

QAction *ActionsManager::action(const QString &name)
{
    auto resultAction = m_collection.action(name);

    if (!resultAction) {
        setupActions(name);
        resultAction = m_collection.action(name);
    }

    return resultAction;
}

void ActionsManager::createUserAction(const QString &text)
{
    KSharedConfig::Ptr m_customCommandsConfig;
    QString ccConfig = Global::instance()->appConfigFilePath(Global::ConfigFile::CustomCommands);
    m_customCommandsConfig = KSharedConfig::openConfig(ccConfig, KConfig::SimpleConfig);
    int counter = m_customCommandsConfig->group(QString()).readEntry("Counter", 0);
    const QString &name = QString("Command_%1").arg(counter);

    auto action = new HAction();
    action->setText(text);
    m_collection.addAction(name, action);
    m_collection.readSettings(m_shortcuts);
}

void ActionsManager::setupUserActions()
{
    KSharedConfig::Ptr m_customCommandsConfig;
    QString ccConfig = Global::instance()->appConfigFilePath(Global::ConfigFile::CustomCommands);
    m_customCommandsConfig = KSharedConfig::openConfig(ccConfig, KConfig::SimpleConfig);
    QStringList groups = m_customCommandsConfig->groupList();
    for (const QString &_group : qAsConst((groups))) {
        auto configGroup = m_customCommandsConfig->group(_group);
        QString command = configGroup.readEntry("Command", QString());

        if (configGroup.readEntry("Type", QString()) == "shortcut") {
            auto action = new HAction();
            action->setText(command);
            m_collection.addAction(_group, action);
        }
    }
    m_collection.readSettings(m_shortcuts);
}

void ActionsManager::configureShortcuts(const QString &name)
{
    auto config = KSharedConfig::openConfig(Global::instance()->appConfigFilePath());
    KShortcutsDialog dlg(KShortcutsEditor::ApplicationAction, KShortcutsEditor::LetterShortcutsAllowed, nullptr);
    connect(&dlg, &KShortcutsDialog::accepted, this, [ = ](){
        m_collection.writeSettings(m_shortcuts);
        config->sync();
    });

    if (!name.isEmpty()) {
        auto searchLine = dlg.findChild<KTreeWidgetSearchLine *>();
        if (searchLine != nullptr) {
            searchLine->setText(name);
        }
    }

    dlg.setModal(true);
    dlg.addCollection(&m_collection);
    dlg.configure(false);
}

void ActionsManager::setupActions(const QString &actionName)
{
    if (actionName == QStringLiteral("screenshot")) {
        auto action = new HAction();
        action->setText(i18n("Screenshot"));
        action->setIcon(QIcon::fromTheme("image-x-generic"));
        m_collection.setDefaultShortcut(action, Qt::Key_S);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("file_quit")) {
        auto action = new HAction();
        action->setText(i18n("Quit"));
        action->setIcon(QIcon::fromTheme("application-exit"));
        connect(action, &QAction::triggered, qApp, &QApplication::quit);
        m_collection.setDefaultShortcut(action, Qt::CTRL + Qt::Key_Q);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("options_configure_keybinding")) {
        auto action = new HAction();
        action->setText(i18n("Configure Keyboard Shortcuts"));
        action->setIcon(QIcon::fromTheme("configure-shortcuts"));
        connect(action, &QAction::triggered, this, [=]() { configureShortcuts(); });
        m_collection.setDefaultShortcut(action, Qt::CTRL + Qt::SHIFT + Qt::Key_S);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("configure")) {
        auto action = new HAction();
        action->setText(i18n("Configure"));
        action->setIcon(QIcon::fromTheme("configure"));
        m_collection.setDefaultShortcut(action, Qt::CTRL + Qt::SHIFT + Qt::Key_Comma);
        m_collection.addAction(actionName, action);
    }

    if (actionName == QStringLiteral("togglePlaylist")) {
        auto action = new HAction();
        action->setText(i18n("Playlist"));
        action->setIcon(QIcon::fromTheme("view-media-playlist"));
        m_collection.setDefaultShortcut(action, Qt::Key_P);
        m_collection.addAction(actionName, action);
    }

    if (actionName == QStringLiteral("openContextMenu")) {
        auto action = new HAction();
        action->setText(i18n("Open Context Menu"));
        action->setIcon(QIcon::fromTheme("application-menu"));
        m_collection.setDefaultShortcut(action, Qt::Key_Menu);
        m_collection.addAction(actionName, action);
    }

    if (actionName == QStringLiteral("toggleFullscreen")) {
        auto action = new HAction();
        action->setText(i18n("Toggle Fullscreen"));
        action->setIcon(QIcon::fromTheme("view-fullscreen"));
        m_collection.setDefaultShortcut(action, Qt::Key_F);
        m_collection.addAction(actionName, action);
    }

    if (actionName == QStringLiteral("openFile")) {
        auto action = new HAction();
        action->setText(i18n("Open File"));
        action->setIcon(QIcon::fromTheme("folder-videos"));
        m_collection.setDefaultShortcut(action, Qt::CTRL + Qt::Key_O);
        m_collection.addAction(actionName, action);
    }

    if (actionName == QStringLiteral("openUrl")) {
        auto action = new HAction();
        action->setText(i18n("Open Url"));
        action->setIcon(QIcon::fromTheme("internet-services"));
        m_collection.setDefaultShortcut(action, Qt::CTRL + Qt::SHIFT + Qt::Key_O);
        m_collection.addAction(actionName, action);
    }

    if (actionName == QStringLiteral("aboutHaruna")) {
        auto action = new HAction();
        action->setText(i18n("About Haruna"));
        action->setIcon(QIcon::fromTheme("help-about"));
        m_collection.setDefaultShortcut(action, Qt::Key_F1);
        m_collection.addAction(actionName, action);
    }

    // mpv actions
    if (actionName == QStringLiteral("contrastUp")) {
        auto action = new HAction();
        action->setText(i18n("Contrast Up"));
        action->setIcon(QIcon::fromTheme("contrast"));
        m_collection.setDefaultShortcut(action, Qt::Key_1);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("contrastDown")) {
        auto action = new HAction();
        action->setText(i18n("Contrast Down"));
        action->setIcon(QIcon::fromTheme("contrast"));
        m_collection.setDefaultShortcut(action, Qt::Key_2);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("contrastReset")) {
        auto action = new HAction();
        action->setText(i18n("Contrast Reset"));
        action->setIcon(QIcon::fromTheme("contrast"));
        m_collection.setDefaultShortcut(action, Qt::CTRL + Qt::Key_1);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("brightnessUp")) {
        auto action = new HAction();
        action->setText(i18n("Brightness Up"));
        action->setIcon(QIcon::fromTheme("contrast"));
        m_collection.setDefaultShortcut(action, Qt::Key_3);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("brightnessDown")) {
        auto action = new HAction();
        action->setText(i18n("Brightness Down"));
        action->setIcon(QIcon::fromTheme("contrast"));
        m_collection.setDefaultShortcut(action, Qt::Key_4);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("brightnessReset")) {
        auto action = new HAction();
        action->setText(i18n("Brightness Reset"));
        action->setIcon(QIcon::fromTheme("contrast"));
        m_collection.setDefaultShortcut(action, Qt::CTRL + Qt::Key_3);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("gammaUp")) {
        auto action = new HAction();
        action->setText(i18n("Gamma Up"));
        action->setIcon(QIcon::fromTheme("contrast"));
        m_collection.setDefaultShortcut(action, Qt::Key_5);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("gammaDown")) {
        auto action = new HAction();
        action->setText(i18n("Gamma Down"));
        action->setIcon(QIcon::fromTheme("contrast"));
        m_collection.setDefaultShortcut(action, Qt::Key_6);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("gammaReset")) {
        auto action = new HAction();
        action->setText(i18n("Gamma Reset"));
        action->setIcon(QIcon::fromTheme("contrast"));
        m_collection.setDefaultShortcut(action, Qt::CTRL + Qt::Key_5);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("saturationUp")) {
        auto action = new HAction();
        action->setText(i18n("Saturation Up"));
        action->setIcon(QIcon::fromTheme("contrast"));
        m_collection.setDefaultShortcut(action, Qt::Key_7);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("saturationDown")) {
        auto action = new HAction();
        action->setText(i18n("Saturation Down"));
        action->setIcon(QIcon::fromTheme("contrast"));
        m_collection.setDefaultShortcut(action, Qt::Key_8);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("saturationReset")) {
        auto action = new HAction();
        action->setText(i18n("Saturation Reset"));
        action->setIcon(QIcon::fromTheme("contrast"));
        m_collection.setDefaultShortcut(action, Qt::CTRL + Qt::Key_7);
        m_collection.addAction(actionName, action);
    }

    if (actionName == QStringLiteral("playNext")) {
        auto action = new HAction();
        action->setText(i18n("Play Next"));
        action->setIcon(QIcon::fromTheme("media-skip-forward"));
        m_collection.setDefaultShortcut(action, Qt::SHIFT + Qt::Key_Period);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("playPrevious")) {
        auto action = new HAction();
        action->setText(i18n("Play Previous"));
        action->setIcon(QIcon::fromTheme("media-skip-backward"));
        m_collection.setDefaultShortcut(action, Qt::SHIFT + Qt::Key_Comma);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("volumeUp")) {
        auto action = new HAction();
        action->setText(i18n("Volume Up"));
        action->setIcon(QIcon::fromTheme("audio-volume-high"));
        m_collection.setDefaultShortcut(action, Qt::Key_9);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("volumeDown")) {
        auto action = new HAction();
        action->setText(i18n("Volume Down"));
        action->setIcon(QIcon::fromTheme("audio-volume-low"));
        m_collection.setDefaultShortcut(action, Qt::Key_0);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("mute")) {
        auto action = new HAction();
        action->setText(i18n("Mute"));
        action->setIcon(QIcon::fromTheme("player-volume"));
        m_collection.setDefaultShortcut(action, Qt::Key_M);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("loadLastPlayedFile")) {
        auto action = new HAction();
        action->setText(i18n("Load Last Played File"));
        action->setIcon(QIcon::fromTheme("document-open-recent"));
        m_collection.setDefaultShortcut(action, Qt::CTRL + Qt::SHIFT + Qt::Key_L);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("restartPlayback")) {
        auto action = new HAction();
        action->setText(i18n("Restart Playback"));
        action->setIcon(QIcon::fromTheme("start-over"));
        m_collection.setDefaultShortcut(action, Qt::Key_F5);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("seekToWatchLaterPosition")) {
        auto action = new HAction();
        action->setText(i18n("Seek To Watch Later Position"));
        action->setIcon(QIcon::fromTheme("pin"));
        m_collection.setDefaultShortcut(action, Qt::CTRL + Qt::SHIFT + Qt::Key_P);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("seekForwardSmall")) {
        auto action = new HAction();
        action->setText(i18n("Seek Small Step Forward"));
        action->setIcon(QIcon::fromTheme("media-seek-forward"));
        m_collection.setDefaultShortcut(action, Qt::Key_Right);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("seekBackwardSmall")) {
        auto action = new HAction();
        action->setText(i18n("Seek Small Step Backward"));
        action->setIcon(QIcon::fromTheme("media-seek-backward"));
        m_collection.setDefaultShortcut(action, Qt::Key_Left);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("seekForwardMedium")) {
        auto action = new HAction();
        action->setText(i18n("Seek Medium Step Forward"));
        action->setIcon(QIcon::fromTheme("media-seek-forward"));
        m_collection.setDefaultShortcut(action, Qt::SHIFT + Qt::Key_Right);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("seekBackwardMedium")) {
        auto action = new HAction();
        action->setText(i18n("Seek Medium Step Backward"));
        action->setIcon(QIcon::fromTheme("media-seek-backward"));
        m_collection.setDefaultShortcut(action, Qt::SHIFT + Qt::Key_Left);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("seekForwardBig")) {
        auto action = new HAction();
        action->setText(i18n("Seek Big Step Forward"));
        action->setIcon(QIcon::fromTheme("media-seek-forward"));
        m_collection.setDefaultShortcut(action, Qt::Key_Up);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("seekBackwardBig")) {
        auto action = new HAction();
        action->setText(i18n("Seek Big Step Backward"));
        action->setIcon(QIcon::fromTheme("media-seek-backward"));
        m_collection.setDefaultShortcut(action, Qt::Key_Down);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("seekPreviousChapter")) {
        auto action = new HAction();
        action->setText(i18n("Seek Previous Chapter"));
        action->setIcon(QIcon::fromTheme("media-seek-backward"));
        m_collection.setDefaultShortcut(action, Qt::Key_PageDown);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("seekNextChapter")) {
        auto action = new HAction();
        action->setText(i18n("Seek Next Chapter"));
        action->setIcon(QIcon::fromTheme("media-seek-forward"));
        m_collection.setDefaultShortcut(action, Qt::Key_PageUp);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("seekNextSubtitle")) {
        auto action = new HAction();
        action->setText(i18n("Seek To Next Subtitle"));
        action->setIcon(QIcon::fromTheme("media-seek-forward"));
        m_collection.setDefaultShortcut(action, Qt::CTRL + Qt::Key_Right);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("seekPreviousSubtitle")) {
        auto action = new HAction();
        action->setText(i18n("Seek To Previous Subtitle"));
        action->setIcon(QIcon::fromTheme("media-seek-backward"));
        m_collection.setDefaultShortcut(action, Qt::CTRL + Qt::Key_Left);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("frameStep")) {
        auto action = new HAction();
        action->setText(i18n("Move one frame forward, then pause"));
        m_collection.setDefaultShortcut(action, Qt::Key_Period);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("frameBackStep")) {
        auto action = new HAction();
        action->setText(i18n("Move one frame backward, then pause"));
        m_collection.setDefaultShortcut(action, Qt::Key_Comma);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("increasePlayBackSpeed")) {
        auto action = new HAction();
        action->setText(i18n("Playback speed increase"));
        m_collection.setDefaultShortcut(action, Qt::Key_BracketRight);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("decreasePlayBackSpeed")) {
        auto action = new HAction();
        action->setText(i18n("Playback speed decrease"));
        m_collection.setDefaultShortcut(action, Qt::Key_BracketLeft);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("resetPlayBackSpeed")) {
        auto action = new HAction();
        action->setText(i18n("Playback speed reset"));
        m_collection.setDefaultShortcut(action, Qt::Key_Backspace);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("subtitleQuicken")) {
        auto action = new HAction();
        action->setText(i18n("Subtitle Quicken"));
        m_collection.setDefaultShortcut(action, Qt::Key_Z);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("subtitleDelay")) {
        auto action = new HAction();
        action->setText(i18n("Subtitle Delay"));
        m_collection.setDefaultShortcut(action, Qt::SHIFT + Qt::Key_Z);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("subtitleToggle")) {
        auto action = new HAction();
        action->setText(i18n("Subtitle Toggle"));
        m_collection.setDefaultShortcut(action, Qt::CTRL + Qt::Key_S);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("audioCycleUp")) {
        auto action = new HAction();
        action->setText(i18n("Cycle Audio Up"));
        m_collection.setDefaultShortcut(action, Qt::SHIFT + Qt::Key_3);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("audioCycleDown")) {
        auto action = new HAction();
        action->setText(i18n("Cycle Audio Down"));
        m_collection.setDefaultShortcut(action, Qt::SHIFT + Qt::Key_2);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("subtitleCycleUp")) {
        auto action = new HAction();
        action->setText(i18n("Cycle Subtitle Up"));
        m_collection.setDefaultShortcut(action, Qt::Key_J);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("subtitleCycleDown")) {
        auto action = new HAction();
        action->setText(i18n("Cycle Subtitle Down"));
        m_collection.setDefaultShortcut(action, Qt::SHIFT + Qt::Key_J);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("zoomIn")) {
        auto action = new HAction();
        action->setText(i18n("Zoom In"));
        action->setIcon(QIcon::fromTheme("zoom-in"));
        m_collection.setDefaultShortcut(action, Qt::ALT + Qt::Key_Plus);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("zoomOut")) {
        auto action = new HAction();
        action->setText(i18n("Zoom Out"));
        action->setIcon(QIcon::fromTheme("zoom-out"));
        m_collection.setDefaultShortcut(action, Qt::ALT + Qt::Key_Minus);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("zoomReset")) {
        auto action = new HAction();
        action->setText(i18n("Zoom Reset"));
        action->setIcon(QIcon::fromTheme("zoom-original"));
        m_collection.setDefaultShortcut(action, Qt::ALT + Qt::Key_Backspace);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("videoPanXLeft")) {
        auto action = new HAction();
        action->setText(i18n("Video pan x left"));
        m_collection.setDefaultShortcut(action, Qt::ALT + Qt::Key_Left);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("videoPanXRight")) {
        auto action = new HAction();
        action->setText(i18n("Video pan x right"));
        m_collection.setDefaultShortcut(action, Qt::ALT + Qt::Key_Right);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("videoPanYUp")) {
        auto action = new HAction();
        action->setText(i18n("Video pan y up"));
        m_collection.setDefaultShortcut(action, Qt::ALT + Qt::Key_Up);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("videoPanYDown")) {
        auto action = new HAction();
        action->setText(i18n("Video pan y down"));
        m_collection.setDefaultShortcut(action, Qt::ALT + Qt::Key_Down);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("toggleMenuBar")) {
        auto action = new HAction();
        action->setText(i18n("Toggle Menu Bar"));
        m_collection.setDefaultShortcut(action, Qt::CTRL + Qt::Key_M);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("toggleHeader")) {
        auto action = new HAction();
        action->setText(i18n("Toggle Header"));
        m_collection.setDefaultShortcut(action, Qt::CTRL + Qt::Key_H);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("setLoop")) {
        auto action = new HAction();
        action->setText(i18n("Set Loop"));
        m_collection.setDefaultShortcut(action, Qt::Key_L);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("increaseSubtitleFontSize")) {
        auto action = new HAction();
        action->setText(i18n("Increase Subtitle Font Size"));
        m_collection.setDefaultShortcut(action, Qt::CTRL + Qt::Key_Z);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("decreaseSubtitleFontSize")) {
        auto action = new HAction();
        action->setText(i18n("Decrease Subtitle Font Size"));
        m_collection.setDefaultShortcut(action, Qt::CTRL + Qt::Key_X);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("subtitlePositionUp")) {
        auto action = new HAction();
        action->setText(i18n("Move Subtitle Up"));
        m_collection.setDefaultShortcut(action, Qt::Key_R);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("subtitlePositionDown")) {
        auto action = new HAction();
        action->setText(i18n("Move Subtitle Down"));
        m_collection.setDefaultShortcut(action, Qt::SHIFT + Qt::Key_R);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("toggleDeinterlacing")) {
        auto action = new HAction();
        action->setText(i18n("Toggle Deinterlacing"));
        m_collection.setDefaultShortcut(action, Qt::Key_D);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("exitFullscreen")) {
        auto action = new HAction();
        action->setText(i18n("Exit Fullscreen"));
        m_collection.setDefaultShortcut(action, Qt::Key_Escape);
        m_collection.addAction(actionName, action);
    }
    m_collection.readSettings(m_shortcuts);
}
