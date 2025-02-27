/*
 * SPDX-FileCopyrightText: 2022 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "actionsmodel.h"

#include <QMessageBox>
#include <QPushButton>

#include <KConfigGroup>

#include "global.h"

using namespace Qt::StringLiterals;

ActionsModel::ActionsModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_config(KSharedConfig::openConfig(Global::instance()->appConfigFilePath(Global::ConfigFile::Shortcuts)))
{
    Action action;
    action.name = u"openActionsDialogAction"_s;
    action.text = i18nc("@action", "Open Actions Dialog");
    action.defaultShortcut = Qt::CTRL | Qt::Key_QuoteLeft; // Key_QuoteLeft =  backtick
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"aboutHarunaAction"_s;
    action.text = i18nc("@action", "About Haruna");
    action.iconName = u"haruna"_s;
    action.defaultShortcut = Qt::Key_F1;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"reportBugAction"_s;
    action.text = i18nc("@action", "Report bug");
    action.iconName = u"tools-report-bug"_s;
    action.defaultShortcut = Qt::CTRL | Qt::SHIFT | Qt::Key_F1;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"sponsorAction"_s;
    action.text = i18nc("@action", "Sponsor Haruna");
    action.iconName = u"help-donate-ars"_s;
    action.defaultShortcut = Qt::Key_Dollar;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"audioCycleUpAction"_s;
    action.text = i18nc("@action", "Audio Cycle Up");
    action.iconName = QString();
    action.defaultShortcut = Qt::SHIFT | Qt::Key_3;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"audioCycleDownAction"_s;
    action.text = i18nc("@action", "Audio Cycle Down");
    action.iconName = QString();
    action.defaultShortcut = Qt::SHIFT | Qt::Key_2;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"configureAction"_s;
    action.text = i18nc("@action", "Settings");
    action.iconName = u"configure"_s;
    action.defaultShortcut = Qt::CTRL | Qt::SHIFT | Qt::Key_Comma;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"configureShortcutsAction"_s;
    action.text = i18nc("@action", "Configure Keyboard Shortcuts");
    action.iconName = u"configure-shortcuts"_s;
    action.defaultShortcut = Qt::CTRL | Qt::ALT | Qt::Key_Comma;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"exitFullscreenAction"_s;
    action.text = i18nc("@action", "Exit Fullscreen");
    action.iconName = QString();
    action.defaultShortcut = Qt::Key_Escape;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"frameStepForwardAction"_s;
    action.text = i18nc("@action", "Frame Step Forward");
    action.iconName = QString();
    action.defaultShortcut = Qt::Key_Period;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = u"Move one frame forward, then pause"_s;
    m_actions << action;

    action.name = u"frameStepBackwardAction"_s;
    action.text = i18nc("@action", "Frame Step Backward");
    action.iconName = QString();
    action.defaultShortcut = Qt::Key_Comma;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = u"Move one frame backward, then pause"_s;
    m_actions << action;

    action.name = u"loadLastPlayedFileAction"_s;
    action.text = i18nc("@action", "Load Last Played File");
    action.iconName = u"document-open-recent"_s;
    action.defaultShortcut = Qt::CTRL | Qt::SHIFT | Qt::Key_L;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"muteAction"_s;
    action.text = i18nc("@action", "Mute");
    action.iconName = u"player-volume-muted"_s;
    action.defaultShortcut = Qt::Key_M;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"openContextMenuAction"_s;
    action.text = i18nc("@action", "Open Context Menu");
    action.iconName = u"application-menu"_s;
    action.defaultShortcut = Qt::Key_Menu;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = i18nc("@action", "Opens and closes the context menu at the position of the mouse pointer");
    m_actions << action;

    action.name = u"openFileAction"_s;
    action.text = i18nc("@action", "Open File");
    action.iconName = u"folder-videos"_s;
    action.defaultShortcut = Qt::CTRL | Qt::Key_O;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"openSubtitlesFileAction"_s;
    action.text = i18nc("@action", "Add External Subtitle…");
    action.iconName = u"add-subtitle"_s;
    action.defaultShortcut = Qt::ALT | Qt::Key_S;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"openUrlAction"_s;
    action.text = i18nc("@action", "Open Url");
    action.iconName = u"internet-services"_s;
    action.defaultShortcut = Qt::CTRL | Qt::SHIFT | Qt::Key_O;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"playbackSpeedIncreaseAction"_s;
    action.text = i18nc("@action", "Playback Speed Increase");
    action.iconName = QString();
    action.defaultShortcut = Qt::Key_BracketRight;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"playbackSpeedDecreaseAction"_s;
    action.text = i18nc("@action", "Playback Speed Decrease");
    action.iconName = QString();
    action.defaultShortcut = Qt::Key_BracketLeft;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"playbackSpeedResetAction"_s;
    action.text = i18nc("@action", "Playback Speed Reset");
    action.iconName = QString();
    action.defaultShortcut = Qt::Key_Backspace;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"playPauseAction"_s;
    action.text = i18nc("@action", "Play/Pause");
    action.iconName = u"media-playback-pause"_s;
    action.defaultShortcut = Qt::Key_Space;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"playNextAction"_s;
    action.text = i18nc("@action", "Play Next");
    action.iconName = u"media-skip-forward"_s;
    action.defaultShortcut = Qt::SHIFT | Qt::Key_Period;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"playPreviousAction"_s;
    action.text = i18nc("@action", "Play Previous");
    action.iconName = u"media-skip-backward"_s;
    action.defaultShortcut = Qt::SHIFT | Qt::Key_Comma;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"quitApplicationAction"_s;
    action.text = i18nc("@action", "Quit");
    action.iconName = u"application-exit"_s;
    action.defaultShortcut = Qt::CTRL | Qt::Key_Q;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"restartPlaybackAction"_s;
    action.text = i18nc("@action", "Restart Playback");
    action.iconName = u"edit-reset"_s;
    action.defaultShortcut = Qt::Key_F5;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"seekForwardSmallAction"_s;
    action.text = i18nc("@action", "Seek Small Step Forward");
    action.iconName = u"media-seek-forward"_s;
    action.defaultShortcut = Qt::Key_Right;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"seekBackwardSmallAction"_s;
    action.text = i18nc("@action", "Seek Small Step Backward");
    action.iconName = u"media-seek-backward"_s;
    action.defaultShortcut = Qt::Key_Left;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"seekForwardMediumAction"_s;
    action.text = i18nc("@action", "Seek Medium Step Forward");
    action.iconName = u"media-seek-forward"_s;
    action.defaultShortcut = Qt::SHIFT | Qt::Key_Right;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"seekBackwardMediumAction"_s;
    action.text = i18nc("@action", "Seek Medium Step Backward");
    action.iconName = u"media-seek-backward"_s;
    action.defaultShortcut = Qt::SHIFT | Qt::Key_Left;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"seekForwardBigAction"_s;
    action.text = i18nc("@action", "Seek Big Step Forward");
    action.iconName = u"media-seek-forward"_s;
    action.defaultShortcut = Qt::Key_Up;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"seekBackwardBigAction"_s;
    action.text = i18nc("@action", "Seek Big Step Backward");
    action.iconName = u"media-seek-backward"_s;
    action.defaultShortcut = Qt::Key_Down;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"seekNextChapterAction"_s;
    action.text = i18nc("@action", "Seek Next Chapter");
    action.iconName = u"media-seek-forward"_s;
    action.defaultShortcut = Qt::Key_PageUp;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"seekPreviousChapterAction"_s;
    action.text = i18nc("@action", "Seek Previous Chapter");
    action.iconName = u"media-seek-backward"_s;
    action.defaultShortcut = Qt::Key_PageDown;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"seekNextSubtitleAction"_s;
    action.text = i18nc("@action", "Seek To Next Subtitle");
    action.iconName = u"media-seek-forward"_s;
    action.defaultShortcut = Qt::CTRL | Qt::Key_Right;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"seekPreviousSubtitleAction"_s;
    action.text = i18nc("@action", "Seek To Previous Subtitle");
    action.iconName = u"media-seek-backward"_s;
    action.defaultShortcut = Qt::CTRL | Qt::Key_Left;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"seekToWatchLaterPositionAction"_s;
    action.text = i18nc("@action", "Seek To Watch Later Position");
    action.iconName = u"pin"_s;
    action.defaultShortcut = Qt::CTRL | Qt::SHIFT | Qt::Key_P;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"setLoopAction"_s;
    action.text = i18nc("@action", "Set Loop");
    action.iconName = QString();
    action.defaultShortcut = Qt::Key_L;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"screenshotAction"_s;
    action.text = i18nc("@action", "Screenshot");
    action.iconName = u"image-x-generic"_s;
    action.defaultShortcut = Qt::Key_S;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"subtitleToggleAction"_s;
    action.text = i18nc("@action", "Subtitle Toggle");
    action.iconName = QString();
    action.defaultShortcut = Qt::CTRL | Qt::Key_S;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"subtitleIncreaseFontSizeAction"_s;
    action.text = i18nc("@action", "Subtitle Increase Font Size");
    action.iconName = QString();
    action.defaultShortcut = Qt::CTRL | Qt::Key_Z;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"subtitleDecreaseFontSizeAction"_s;
    action.text = i18nc("@action", "Subtitle Decrease Font Size");
    action.iconName = QString();
    action.defaultShortcut = Qt::CTRL | Qt::Key_X;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"subtitleMoveUpAction"_s;
    action.text = i18nc("@action", "Subtitle Move Up");
    action.iconName = QString();
    action.defaultShortcut = Qt::Key_R;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"subtitleMoveDownAction"_s;
    action.text = i18nc("@action", "Subtitle Move Down");
    action.iconName = QString();
    action.defaultShortcut = Qt::SHIFT | Qt::Key_R;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"subtitleQuickenAction"_s;
    action.text = i18nc("@action", "Subtitle Quicken");
    action.iconName = QString();
    action.defaultShortcut = Qt::Key_Z;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"subtitleDelayAction"_s;
    action.text = i18nc("@action", "Subtitle Delay");
    action.iconName = QString();
    action.defaultShortcut = Qt::SHIFT | Qt::Key_Z;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"subtitleCycleUpAction"_s;
    action.text = i18nc("@action", "Subtitle Cycle Up");
    action.iconName = QString();
    action.defaultShortcut = Qt::Key_J;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"subtitleCycleDownAction"_s;
    action.text = i18nc("@action", "Subtitle Cycle Down");
    action.iconName = QString();
    action.defaultShortcut = Qt::SHIFT | Qt::Key_J;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"toggleDeinterlacingAction"_s;
    action.text = i18nc("@action", "Toggle Deinterlacing");
    action.iconName = QString();
    action.defaultShortcut = Qt::Key_D;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"toggleFullscreenAction"_s;
    action.text = i18nc("@action", "Toggle Fullscreen");
    action.iconName = u"view-fullscreen"_s;
    action.defaultShortcut = Qt::Key_F;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"toggleMenuBarAction"_s;
    action.text = i18nc("@action", "Toggle Menu Bar");
    action.iconName = QString();
    action.defaultShortcut = Qt::CTRL | Qt::Key_M;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"toggleHeaderAction"_s;
    action.text = i18nc("@action", "Toggle Header");
    action.iconName = QString();
    action.defaultShortcut = Qt::CTRL | Qt::Key_H;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"togglePlaylistAction"_s;
    action.text = i18nc("@action", "Playlist");
    action.iconName = u"view-media-playlist"_s;
    action.defaultShortcut = Qt::Key_P;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"videoPanXLeftAction"_s;
    action.text = i18nc("@action", "Video Move Left");
    action.iconName = QString();
    action.defaultShortcut = Qt::CTRL | Qt::ALT | Qt::Key_Left;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"videoPanXRightAction"_s;
    action.text = i18nc("@action", "Video Move Right");
    action.iconName = QString();
    action.defaultShortcut = Qt::CTRL | Qt::ALT | Qt::Key_Right;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"videoPanYUpAction"_s;
    action.text = i18nc("@action", "Video Move Up");
    action.iconName = QString();
    action.defaultShortcut = Qt::CTRL | Qt::ALT | Qt::Key_Up;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"videoPanYDownAction"_s;
    action.text = i18nc("@action", "Video Move Down");
    action.iconName = QString();
    action.defaultShortcut = Qt::CTRL | Qt::ALT | Qt::Key_Down;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"rotateClockwiseAction"_s;
    action.text = i18nc("@action", "Rotate clockwise");
    action.iconName = QString();
    action.defaultShortcut = Qt::CTRL | Qt::Key_R;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"rotateCounterClockwiseAction"_s;
    action.text = i18nc("@action", "Rotate counter clockwise");
    action.iconName = QString();
    action.defaultShortcut = Qt::CTRL | Qt::Key_E;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"volumeUpAction"_s;
    action.text = i18nc("@action", "Volume Up");
    action.iconName = u"audio-volume-high"_s;
    action.defaultShortcut = Qt::Key_9;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"volumeDownAction"_s;
    action.text = i18nc("@action", "Volume Down");
    action.iconName = u"audio-volume-low"_s;
    action.defaultShortcut = Qt::Key_0;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"zoomInAction"_s;
    action.text = i18nc("@action", "Zoom In");
    action.iconName = u"zoom-in"_s;
    action.defaultShortcut = Qt::CTRL | Qt::Key_Plus;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"zoomOutAction"_s;
    action.text = i18nc("@action", "Zoom Out");
    action.iconName = u"zoom-out"_s;
    action.defaultShortcut = Qt::CTRL | Qt::Key_Minus;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"zoomResetAction"_s;
    action.text = i18nc("@action", "Zoom Reset");
    action.iconName = u"zoom-original"_s;
    action.defaultShortcut = Qt::CTRL | Qt::Key_0;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"contrastUpAction"_s;
    action.text = i18nc("@action", "Contrast Up");
    action.iconName = u"contrast"_s;
    action.defaultShortcut = Qt::Key_1;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"contrastDownAction"_s;
    action.text = i18nc("@action", "Contrast Down");
    action.iconName = u"contrast"_s;
    action.defaultShortcut = Qt::Key_2;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"contrastResetAction"_s;
    action.text = i18nc("@action", "Contrast Reset");
    action.iconName = u"contrast"_s;
    action.defaultShortcut = Qt::CTRL | Qt::Key_1;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"brightnessUpAction"_s;
    action.text = i18nc("@action", "Brightness Up");
    action.iconName = u"contrast"_s;
    action.defaultShortcut = Qt::Key_3;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"brightnessDownAction"_s;
    action.text = i18nc("@action", "Brightness Down");
    action.iconName = u"contrast"_s;
    action.defaultShortcut = Qt::Key_4;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"brightnessResetAction"_s;
    action.text = i18nc("@action", "Brightness Reset");
    action.iconName = u"contrast"_s;
    action.defaultShortcut = Qt::CTRL | Qt::Key_3;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"gammaUpAction"_s;
    action.text = i18nc("@action", "Gamma Up");
    action.iconName = u"contrast"_s;
    action.defaultShortcut = Qt::Key_5;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"gammaDownAction"_s;
    action.text = i18nc("@action", "Gamma Down");
    action.iconName = u"contrast"_s;
    action.defaultShortcut = Qt::Key_6;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"gammaResetAction"_s;
    action.text = i18nc("@action", "Gamma Reset");
    action.iconName = u"contrast"_s;
    action.defaultShortcut = Qt::CTRL | Qt::Key_5;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"saturationUpAction"_s;
    action.text = i18nc("@action", "Saturation Up");
    action.iconName = u"contrast"_s;
    action.defaultShortcut = Qt::Key_7;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"saturationDownAction"_s;
    action.text = i18nc("@action", "Saturation Down");
    action.iconName = u"contrast"_s;
    action.defaultShortcut = Qt::Key_8;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = u"saturationResetAction"_s;
    action.text = i18nc("@action", "Saturation Reset");
    action.iconName = u"contrast"_s;
    action.defaultShortcut = Qt::CTRL | Qt::Key_7;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;
}

int ActionsModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid()) {
        return 0;
    }

    return m_actions.count();
}

QVariant ActionsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    auto action = m_actions.at(index.row());

    switch (role) {
    case NameRole:
        return QVariant(action.name);
    case TextRole:
        return QVariant(action.text);
    case IconRole:
        return QVariant(action.iconName);
    case ShortcutRole:
        return QVariant(action.shortcut.toString(QKeySequence::PortableText));
    case DefaultShortcutRole:
        return QVariant(action.defaultShortcut.toString(QKeySequence::PortableText));
    case DescriptionRole:
        return QVariant(action.description);
    case TypeRole:
        return QVariant(action.type);
    }

    return QVariant();
}

QHash<int, QByteArray> ActionsModel::roleNames() const
{
    // clang-format off
    QHash<int, QByteArray> roles{
        {NameRole,            QByteArrayLiteral("actionName")},
        {TextRole,            QByteArrayLiteral("actionText")},
        {IconRole,            QByteArrayLiteral("actionIcon")},
        {ShortcutRole,        QByteArrayLiteral("actionShortcut")},
        {DefaultShortcutRole, QByteArrayLiteral("actionDefaultShortcut")},
        {DescriptionRole,     QByteArrayLiteral("actionDescription")},
        {TypeRole,            QByteArrayLiteral("actionType")},
    };
    // clang-format on

    return roles;
}

void ActionsModel::appendCustomAction(const Action &action)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_actions.append(action);
    endInsertRows();
}

void ActionsModel::editCustomAction(const QString &name, const QString &text, const QString &description)
{
    for (int i{0}; i < m_actions.count(); ++i) {
        if (m_actions[i].name == name) {
            m_actions[i].text = text;
            m_actions[i].description = description;
            Q_EMIT dataChanged(index(i, 0), index(i, 0));
            return;
        }
    }
}

bool ActionsModel::saveShortcut(const QString &name, QKeySequence keySequence)
{
    for (int i{0}; i < m_actions.count(); ++i) {
        if (m_actions[i].name == name) {
            return saveShortcut(i, keySequence);
        }
    }
    return false;
}

bool ActionsModel::saveShortcut(int row, QKeySequence keySequence)
{
    auto group = m_config->group(u"Shortcuts"_s);
    // action whose shortcut is changed
    auto action = &m_actions[row];

    // if shortcut is being cleared, no need to search for a conflict
    if (!keySequence.isEmpty()) {
        // if shortcut is used, this is the action holding the shortcut
        Action *result = nullptr;
        int i{0};
        for (; i < m_actions.count(); ++i) {
            if (m_actions[i].shortcut == keySequence) {
                result = &m_actions[i];
                break;
            }
        }

        if (result != nullptr && action->name == result->name) {
            return false;
        }

        if (result != nullptr) {
            // shortcut is used by an action
            // ask user what whether to reassign or to cancel
            if (keyConflictMessageBox(result->text)) {
                // user chose reassign, remove shortcut from action holding it
                result->shortcut = QKeySequence{};
                group.writeEntry(result->name, QString{});
                Q_EMIT shortcutChanged(result->name, result->shortcut);
                Q_EMIT dataChanged(index(i, 0), index(i, 0));
            } else {
                // user chose cancel
                return false;
            }
        }
    }
    // set shortcut on the action being changed
    action->shortcut = keySequence;
    group.writeEntry(action->name, action->shortcut.toString(QKeySequence::PortableText));
    Q_EMIT shortcutChanged(action->name, action->shortcut);
    Q_EMIT dataChanged(index(row, 0), index(row, 0));

    return group.sync();
}

QKeySequence ActionsModel::getShortcut(const QString &key, const QKeySequence &defaultValue) const
{
    auto v = defaultValue.toString(QKeySequence::PortableText);
    auto group = m_config->group(u"Shortcuts"_s);
    QKeySequence keySequence(group.readEntry(key, v), QKeySequence::PortableText);
    return keySequence;
}

bool ActionsModel::keyConflictMessageBox(const QString &actionText)
{
    auto message = i18nc("@info",
                         "The chosen shortcut conflicts with "
                         "the shortcut of the <b>%1</b> action.<br><br>"
                         "Do you want to reassign the shortcut?",
                         actionText);
    QMessageBox msgBox;
    msgBox.setText(message);
    QPushButton *reassignBtn = msgBox.addButton(i18nc("@action:button", "Reassign"), QMessageBox::AcceptRole);
    msgBox.addButton(QMessageBox::Cancel);
    msgBox.exec();

    if (msgBox.clickedButton() == reassignBtn) {
        return true;
    }
    return false;
}

QList<Action> &ActionsModel::actions()
{
    return m_actions;
}

void ActionsModel::signalEmitter(const QString &actionName)
{
    QMetaObject::invokeMethod(this, actionName.toUtf8().data());
}

ProxyActionsModel::ProxyActionsModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setDynamicSortFilter(true);
    setFilterCaseSensitivity(Qt::CaseInsensitive);

    m_nameRegExp.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    m_textRegExp.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    m_typeRegExp.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
}

bool ProxyActionsModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex nameIndex = sourceModel()->index(sourceRow, 0, sourceParent);
    QModelIndex typeIndex = sourceModel()->index(sourceRow, 0, sourceParent);

    QString name = sourceModel()->data(nameIndex, ActionsModel::NameRole).toString();
    QString text = sourceModel()->data(nameIndex, ActionsModel::TextRole).toString();
    QString type = sourceModel()->data(typeIndex, ActionsModel::TypeRole).toString();

    return ((name.contains(m_nameRegExp) || text.contains(m_nameRegExp)) && type.contains(m_typeRegExp));
}

void ProxyActionsModel::setNameFilter(const QString &regExp)
{
    m_nameRegExp.setPattern(regExp);
    invalidateFilter();
}

void ProxyActionsModel::setTypeFilter(const QString &regExp)
{
    m_typeRegExp.setPattern(regExp);
    invalidateFilter();
}

bool ProxyActionsModel::saveShortcut(int row, QKeySequence keySequence)
{
    auto actionsModel = qobject_cast<ActionsModel *>(sourceModel());
    return actionsModel->saveShortcut(mapToSource(index(row, 0)).row(), keySequence);
}

#include "moc_actionsmodel.cpp"
