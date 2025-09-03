/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "_debug.h"
#include "application.h"

#include <QApplication>
#include <QAction>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>

#include <KConfig>
#include <KConfigGroup>
#include <KLocalizedString>
#include <KShortcutsDialog>

Application::Application(QObject *parent)
    : m_collection(parent)
{
    Q_UNUSED(parent)

    m_config = KSharedConfig::openConfig("georgefb/haruna.conf");
    m_shortcuts = new KConfigGroup(m_config, "Shortcuts");
}

QString Application::argument(int key)
{
    return args[key];
}

void Application::addArgument(int key, QString value)
{
    args.insert(key, value);
}

QUrl Application::getPathFromArg(QString arg)
{
    return QUrl::fromUserInput(arg, QDir::currentPath());
}

QAction *Application::action(const QString &name)
{
    auto resultAction = m_collection.action(name);

    if (!resultAction) {
        setupActions(name);
        resultAction = m_collection.action(name);
    }

    return resultAction;
}

QString Application::iconName(const QIcon &icon)
{
    return icon.name();
}

void Application::configureShortcuts()
{
    KShortcutsDialog dlg(KShortcutsEditor::ApplicationAction, KShortcutsEditor::LetterShortcutsAllowed, nullptr);
    connect(&dlg, &KShortcutsDialog::accepted, this, [ = ](){
        m_collection.writeSettings(m_shortcuts);
        m_config->sync();
    });
    dlg.setModal(true);
    dlg.addCollection(&m_collection);
    dlg.configure(false);
}

void Application::hideCursor()
{
    QApplication::setOverrideCursor(Qt::BlankCursor);
}

void Application::showCursor()
{
    QApplication::setOverrideCursor(Qt::ArrowCursor);
}

void Application::setupActions(const QString &actionName)
{
    if (actionName == QStringLiteral("file_quit")) {
        auto action = KStandardAction::quit(QCoreApplication::instance(), &QCoreApplication::quit, &m_collection);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("options_configure_keybinding")) {
        auto action = KStandardAction::keyBindings(this, &Application::configureShortcuts, &m_collection);
        m_collection.setDefaultShortcut(action, Qt::CTRL | Qt::SHIFT | Qt::Key_S);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("configure")) {
        QAction *action = new QAction();
        action->setText(i18n("Configure"));
        action->setIcon(QIcon::fromTheme("configure"));
        m_collection.setDefaultShortcut(action, Qt::CTRL | Qt::SHIFT | Qt::Key_Comma);
        m_collection.addAction(actionName, action);
    }

    if (actionName == QStringLiteral("togglePlaylist")) {
        QAction *action = new QAction();
        action->setText(i18n("Toggle Playlist"));
        action->setIcon(QIcon::fromTheme("view-media-playlist"));
        m_collection.setDefaultShortcut(action, Qt::Key_P);
        m_collection.addAction(actionName, action);
    }

    if (actionName == QStringLiteral("openContextMenu")) {
        QAction *action = new QAction();
        action->setText(i18n("Open Context Menu"));
        action->setIcon(QIcon::fromTheme("application-menu"));
        m_collection.setDefaultShortcut(action, Qt::Key_Menu);
        m_collection.addAction(actionName, action);
    }

    if (actionName == QStringLiteral("toggleFullscreen")) {
        QAction *action = new QAction();
        action->setText(i18n("Toggle Fullscreen"));
        action->setIcon(QIcon::fromTheme("view-fullscreen"));
        m_collection.setDefaultShortcut(action, Qt::Key_F);
        m_collection.addAction(actionName, action);
    }

    if (actionName == QStringLiteral("openFile")) {
        QAction *action = new QAction();
        action->setText(i18n("Open File"));
        action->setIcon(QIcon::fromTheme("folder-videos-symbolic"));
        m_collection.setDefaultShortcut(action, Qt::CTRL | Qt::Key_O);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("openUrl")) {
        QAction *action = new QAction();
        action->setText(i18n("Open Url"));
        action->setIcon(QIcon::fromTheme("internet-services"));
        m_collection.setDefaultShortcut(action, Qt::CTRL | Qt::SHIFT | Qt::Key_O);
        m_collection.addAction(actionName, action);
    }

    // mpv actions
    if (actionName == QStringLiteral("contrastUp")) {
        QAction *action = new QAction();
        action->setText(i18n("Contrast Up"));
        action->setIcon(QIcon::fromTheme("contrast"));
        m_collection.setDefaultShortcut(action, Qt::Key_1);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("contrastDown")) {
        QAction *action = new QAction();
        action->setText(i18n("Contrast Down"));
        action->setIcon(QIcon::fromTheme("contrast"));
        m_collection.setDefaultShortcut(action, Qt::Key_2);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("contrastReset")) {
        QAction *action = new QAction();
        action->setText(i18n("Contrast Reset"));
        action->setIcon(QIcon::fromTheme("contrast"));
        m_collection.setDefaultShortcut(action, Qt::CTRL | Qt::Key_1);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("brightnessUp")) {
        QAction *action = new QAction();
        action->setText(i18n("Brightness Up"));
        action->setIcon(QIcon::fromTheme("contrast"));
        m_collection.setDefaultShortcut(action, Qt::Key_3);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("brightnessDown")) {
        QAction *action = new QAction();
        action->setText(i18n("Brightness Down"));
        action->setIcon(QIcon::fromTheme("contrast"));
        m_collection.setDefaultShortcut(action, Qt::Key_4);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("brightnessReset")) {
        QAction *action = new QAction();
        action->setText(i18n("Brightness Reset"));
        action->setIcon(QIcon::fromTheme("contrast"));
        m_collection.setDefaultShortcut(action, Qt::CTRL | Qt::Key_3);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("gammaUp")) {
        QAction *action = new QAction();
        action->setText(i18n("Gamma Up"));
        action->setIcon(QIcon::fromTheme("contrast"));
        m_collection.setDefaultShortcut(action, Qt::Key_5);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("gammaDown")) {
        QAction *action = new QAction();
        action->setText(i18n("Gamma Down"));
        action->setIcon(QIcon::fromTheme("contrast"));
        m_collection.setDefaultShortcut(action, Qt::Key_6);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("gammaReset")) {
        QAction *action = new QAction();
        action->setText(i18n("Gamma Reset"));
        action->setIcon(QIcon::fromTheme("contrast"));
        m_collection.setDefaultShortcut(action, Qt::CTRL | Qt::Key_5);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("saturationUp")) {
        QAction *action = new QAction();
        action->setText(i18n("Saturation Up"));
        action->setIcon(QIcon::fromTheme("contrast"));
        m_collection.setDefaultShortcut(action, Qt::Key_7);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("saturationDown")) {
        QAction *action = new QAction();
        action->setText(i18n("Saturation Down"));
        action->setIcon(QIcon::fromTheme("contrast"));
        m_collection.setDefaultShortcut(action, Qt::Key_8);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("saturationReset")) {
        QAction *action = new QAction();
        action->setText(i18n("Saturation Reset"));
        action->setIcon(QIcon::fromTheme("contrast"));
        m_collection.setDefaultShortcut(action, Qt::CTRL | Qt::Key_7);
        m_collection.addAction(actionName, action);
    }

    if (actionName == QStringLiteral("playNext")) {
        QAction *action = new QAction();
        action->setText(i18n("Play Next"));
        action->setIcon(QIcon::fromTheme("media-skip-forward"));
        m_collection.setDefaultShortcut(action, Qt::SHIFT | Qt::Key_Period);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("playPrevious")) {
        QAction *action = new QAction();
        action->setText(i18n("Play Previous"));
        action->setIcon(QIcon::fromTheme("media-skip-backward"));
        m_collection.setDefaultShortcut(action, Qt::SHIFT | Qt::Key_Comma);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("volumeUp")) {
        QAction *action = new QAction();
        action->setText(i18n("Volume Up"));
        action->setIcon(QIcon::fromTheme("audio-volume-high"));
        m_collection.setDefaultShortcut(action, Qt::Key_9);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("volumeDown")) {
        QAction *action = new QAction();
        action->setText(i18n("Volume Down"));
        action->setIcon(QIcon::fromTheme("audio-volume-low"));
        m_collection.setDefaultShortcut(action, Qt::Key_0);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("mute")) {
        QAction *action = new QAction();
        action->setText(i18n("Mute"));
        action->setIcon(QIcon::fromTheme("player-volume"));
        m_collection.setDefaultShortcut(action, Qt::Key_M);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("seekForwardSmall")) {
        QAction *action = new QAction();
        action->setText(i18n("Seek Small Step Forward"));
        action->setIcon(QIcon::fromTheme("media-seek-forward"));
        m_collection.setDefaultShortcut(action, Qt::Key_Right);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("seekBackwardSmall")) {
        QAction *action = new QAction();
        action->setText(i18n("Seek Small Step Backward"));
        action->setIcon(QIcon::fromTheme("media-seek-backward"));
        m_collection.setDefaultShortcut(action, Qt::Key_Left);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("seekForwardMedium")) {
        QAction *action = new QAction();
        action->setText(i18n("Seek Medium Step Forward"));
        action->setIcon(QIcon::fromTheme("media-seek-forward"));
        m_collection.setDefaultShortcut(action, Qt::SHIFT | Qt::Key_Right);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("seekBackwardMedium")) {
        QAction *action = new QAction();
        action->setText(i18n("Seek Medium Step Backward"));
        action->setIcon(QIcon::fromTheme("media-seek-backward"));
        m_collection.setDefaultShortcut(action, Qt::SHIFT | Qt::Key_Left);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("seekForwardBig")) {
        QAction *action = new QAction();
        action->setText(i18n("Seek Big Step Forward"));
        action->setIcon(QIcon::fromTheme("media-seek-forward"));
        m_collection.setDefaultShortcut(action, Qt::Key_Up);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("seekBackwardBig")) {
        QAction *action = new QAction();
        action->setText(i18n("Seek Big Step Backward"));
        action->setIcon(QIcon::fromTheme("media-seek-backward"));
        m_collection.setDefaultShortcut(action, Qt::Key_Down);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("seekPreviousChapter")) {
        QAction *action = new QAction();
        action->setText(i18n("Seek Previous Chapter"));
        action->setIcon(QIcon::fromTheme("media-seek-backward"));
        m_collection.setDefaultShortcut(action, Qt::Key_PageDown);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("seekNextChapter")) {
        QAction *action = new QAction();
        action->setText(i18n("Seek Next Chapter"));
        action->setIcon(QIcon::fromTheme("media-seek-forward"));
        m_collection.setDefaultShortcut(action, Qt::Key_PageUp);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("seekNextSubtitle")) {
        QAction *action = new QAction();
        action->setText(i18n("Seek To Next Subtitle"));
        action->setIcon(QIcon::fromTheme("media-seek-forward"));
        m_collection.setDefaultShortcut(action, Qt::CTRL | Qt::Key_Right);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("seekPreviousSubtitle")) {
        QAction *action = new QAction();
        action->setText(i18n("Seek To Previous Subtitle"));
        action->setIcon(QIcon::fromTheme("media-seek-backward"));
        m_collection.setDefaultShortcut(action, Qt::CTRL | Qt::Key_Left);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("frameStep")) {
        QAction *action = new QAction();
        action->setText(i18n("Move one frame forward, then pause"));
        m_collection.setDefaultShortcut(action, Qt::Key_Period);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("frameBackStep")) {
        QAction *action = new QAction();
        action->setText(i18n("Move one frame backward, then pause"));
        m_collection.setDefaultShortcut(action, Qt::Key_Comma);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("increasePlayBackSpeed")) {
        QAction *action = new QAction();
        action->setText(i18n("Playback speed increase"));
        m_collection.setDefaultShortcut(action, Qt::Key_BracketRight);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("decreasePlayBackSpeed")) {
        QAction *action = new QAction();
        action->setText(i18n("Playback speed decrease"));
        m_collection.setDefaultShortcut(action, Qt::Key_BracketLeft);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("resetPlayBackSpeed")) {
        QAction *action = new QAction();
        action->setText(i18n("Playback speed reset"));
        m_collection.setDefaultShortcut(action, Qt::Key_Backspace);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("subtitleQuicken")) {
        QAction *action = new QAction();
        action->setText(i18n("Subtitle Quicken"));
        m_collection.setDefaultShortcut(action, Qt::Key_Z);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("subtitleDelay")) {
        QAction *action = new QAction();
        action->setText(i18n("Subtitle Delay"));
        m_collection.setDefaultShortcut(action, Qt::SHIFT | Qt::Key_Z);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("subtitleToggle")) {
        QAction *action = new QAction();
        action->setText(i18n("Subtitle Toggle"));
        m_collection.setDefaultShortcut(action, Qt::CTRL | Qt::Key_S);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("zoomIn")) {
        QAction *action = new QAction();
        action->setText(i18n("Zoom In"));
        action->setIcon(QIcon::fromTheme("zoom-in"));
        m_collection.setDefaultShortcut(action, Qt::ALT | Qt::Key_Plus);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("zoomOut")) {
        QAction *action = new QAction();
        action->setText(i18n("Zoom Out"));
        action->setIcon(QIcon::fromTheme("zoom-out"));
        m_collection.setDefaultShortcut(action, Qt::ALT | Qt::Key_Minus);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("zoomReset")) {
        QAction *action = new QAction();
        action->setText(i18n("Zoom Reset"));
        action->setIcon(QIcon::fromTheme("zoom-original"));
        m_collection.setDefaultShortcut(action, Qt::ALT | Qt::Key_Backspace);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("videoPanXLeft")) {
        QAction *action = new QAction();
        action->setText(i18n("Video pan x left"));
        m_collection.setDefaultShortcut(action, Qt::ALT | Qt::Key_Left);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("videoPanXRight")) {
        QAction *action = new QAction();
        action->setText(i18n("Video pan x right"));
        m_collection.setDefaultShortcut(action, Qt::ALT | Qt::Key_Right);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("videoPanYUp")) {
        QAction *action = new QAction();
        action->setText(i18n("Video pan y up"));
        m_collection.setDefaultShortcut(action, Qt::ALT | Qt::Key_Up);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("videoPanYDown")) {
        QAction *action = new QAction();
        action->setText(i18n("Video pan y down"));
        m_collection.setDefaultShortcut(action, Qt::ALT | Qt::Key_Down);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("toggleMenuBar")) {
        QAction *action = new QAction();
        action->setText(i18n("Toggle Menu Bar"));
        m_collection.setDefaultShortcut(action, Qt::CTRL | Qt::Key_M);
        m_collection.addAction(actionName, action);
    }
    if (actionName == QStringLiteral("toggleHeader")) {
        QAction *action = new QAction();
        action->setText(i18n("Toggle Header"));
        m_collection.setDefaultShortcut(action, Qt::CTRL | Qt::Key_H);
        m_collection.addAction(actionName, action);
    }
    m_collection.readSettings(m_shortcuts);
}
