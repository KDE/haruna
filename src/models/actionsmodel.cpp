/*
 * SPDX-FileCopyrightText: 2022 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "actionsmodel.h"

#include <KConfigGroup>

#include <QMessageBox>
#include <QPushButton>

#include "global.h"

ActionsModel::ActionsModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_config(KSharedConfig::openConfig(Global::instance()->appConfigFilePath(Global::ConfigFile::Shortcuts)))
{
    Action action;
    action.name = QStringLiteral("openActionsDialogAction");
    action.text = i18nc("@action", "Open Actions Dialog");
    action.defaultShortcut = Qt::CTRL | Qt::Key_QuoteLeft; // Key_QuoteLeft =  backtick
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("aboutHarunaAction");
    action.text = i18nc("@action", "About Haruna");
    action.iconName = QStringLiteral("haruna");
    action.defaultShortcut = Qt::Key_F1;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("audioCycleUpAction");
    action.text = i18nc("@action", "Audio Cycle Up");
    action.iconName = QString();
    action.defaultShortcut = Qt::SHIFT | Qt::Key_3;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("audioCycleDownAction");
    action.text = i18nc("@action", "Audio Cycle Down");
    action.iconName = QString();
    action.defaultShortcut = Qt::SHIFT | Qt::Key_2;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("configureAction");
    action.text = i18nc("@action", "Configure");
    action.iconName = QStringLiteral("configure");
    action.defaultShortcut = Qt::CTRL | Qt::SHIFT | Qt::Key_Comma;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("configureShortcutsAction");
    action.text = i18nc("@action", "Configure Keyboard Shortcuts");
    action.iconName = QStringLiteral("configure-shortcuts");
    action.defaultShortcut = Qt::CTRL | Qt::ALT | Qt::Key_Comma;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("exitFullscreenAction");
    action.text = i18nc("@action", "Exit Fullscreen");
    action.iconName = QString();
    action.defaultShortcut = Qt::Key_Escape;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("frameStepForwardAction");
    action.text = i18nc("@action", "Frame Step Forward");
    action.iconName = QString();
    action.defaultShortcut = Qt::Key_Period;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QStringLiteral("Move one frame forward, then pause");
    m_actions << action;

    action.name = QStringLiteral("frameStepBackwardAction");
    action.text = i18nc("@action", "Frame Step Backward");
    action.iconName = QString();
    action.defaultShortcut = Qt::Key_Comma;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QStringLiteral("Move one frame backward, then pause");
    m_actions << action;

    action.name = QStringLiteral("loadLastPlayedFileAction");
    action.text = i18nc("@action", "Load Last Played File");
    action.iconName = QStringLiteral("document-open-recent");
    action.defaultShortcut = Qt::CTRL | Qt::SHIFT | Qt::Key_L;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("muteAction");
    action.text = i18nc("@action", "Mute");
    action.iconName = QStringLiteral("player-volume-muted");
    action.defaultShortcut = Qt::Key_M;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("openContextMenuAction");
    action.text = i18nc("@action", "Open Context Menu");
    action.iconName = QStringLiteral("application-menu");
    action.defaultShortcut = Qt::Key_Menu;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = i18nc("@action", "Opens and closes the context menu at the position of the mouse pointer");
    m_actions << action;

    action.name = QStringLiteral("openFileAction");
    action.text = i18nc("@action", "Open File");
    action.iconName = QStringLiteral("folder-videos");
    action.defaultShortcut = Qt::CTRL | Qt::Key_O;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("openSubtitlesFileAction");
    action.text = i18nc("@action", "Add External Subtitle…");
    action.iconName = QStringLiteral("add-subtitle");
    action.defaultShortcut = Qt::ALT | Qt::Key_S;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("openUrlAction");
    action.text = i18nc("@action", "Open Url");
    action.iconName = QStringLiteral("internet-services");
    action.defaultShortcut = Qt::CTRL | Qt::SHIFT | Qt::Key_O;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("playbackSpeedIncreaseAction");
    action.text = i18nc("@action", "Playback Speed Increase");
    action.iconName = QString();
    action.defaultShortcut = Qt::Key_BracketRight;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("playbackSpeedDecreaseAction");
    action.text = i18nc("@action", "Playback Speed Decrease");
    action.iconName = QString();
    action.defaultShortcut = Qt::Key_BracketLeft;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("playbackSpeedResetAction");
    action.text = i18nc("@action", "Playback Speed Reset");
    action.iconName = QString();
    action.defaultShortcut = Qt::Key_Backspace;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("playPauseAction");
    action.text = i18nc("@action", "Play/Pause");
    action.iconName = QStringLiteral("media-playback-pause");
    action.defaultShortcut = Qt::Key_Space;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("playNextAction");
    action.text = i18nc("@action", "Play Next");
    action.iconName = QStringLiteral("media-skip-forward");
    action.defaultShortcut = Qt::SHIFT | Qt::Key_Period;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("playPreviousAction");
    action.text = i18nc("@action", "Play Previous");
    action.iconName = QStringLiteral("media-skip-backward");
    action.defaultShortcut = Qt::SHIFT | Qt::Key_Comma;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("quitApplicationAction");
    action.text = i18nc("@action", "Quit");
    action.iconName = QStringLiteral("application-exit");
    action.defaultShortcut = Qt::CTRL | Qt::Key_Q;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("restartPlaybackAction");
    action.text = i18nc("@action", "Restart Playback");
    action.iconName = QStringLiteral("edit-reset");
    action.defaultShortcut = Qt::Key_F5;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("seekForwardSmallAction");
    action.text = i18nc("@action", "Seek Small Step Forward");
    action.iconName = QStringLiteral("media-seek-forward");
    action.defaultShortcut = Qt::Key_Right;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("seekBackwardSmallAction");
    action.text = i18nc("@action", "Seek Small Step Backward");
    action.iconName = QStringLiteral("media-seek-backward");
    action.defaultShortcut = Qt::Key_Left;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("seekForwardMediumAction");
    action.text = i18nc("@action", "Seek Medium Step Forward");
    action.iconName = QStringLiteral("media-seek-forward");
    action.defaultShortcut = Qt::SHIFT | Qt::Key_Right;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("seekBackwardMediumAction");
    action.text = i18nc("@action", "Seek Medium Step Backward");
    action.iconName = QStringLiteral("media-seek-backward");
    action.defaultShortcut = Qt::SHIFT | Qt::Key_Left;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("seekForwardBigAction");
    action.text = i18nc("@action", "Seek Big Step Forward");
    action.iconName = QStringLiteral("media-seek-forward");
    action.defaultShortcut = Qt::Key_Up;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("seekBackwardBigAction");
    action.text = i18nc("@action", "Seek Big Step Backward");
    action.iconName = QStringLiteral("media-seek-backward");
    action.defaultShortcut = Qt::Key_Down;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("seekNextChapterAction");
    action.text = i18nc("@action", "Seek Next Chapter");
    action.iconName = QStringLiteral("media-seek-forward");
    action.defaultShortcut = Qt::Key_PageUp;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("seekPreviousChapterAction");
    action.text = i18nc("@action", "Seek Previous Chapter");
    action.iconName = QStringLiteral("media-seek-backward");
    action.defaultShortcut = Qt::Key_PageDown;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("seekNextSubtitleAction");
    action.text = i18nc("@action", "Seek To Next Subtitle");
    action.iconName = QStringLiteral("media-seek-forward");
    action.defaultShortcut = Qt::CTRL | Qt::Key_Right;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("seekPreviousSubtitleAction");
    action.text = i18nc("@action", "Seek To Previous Subtitle");
    action.iconName = QStringLiteral("media-seek-backward");
    action.defaultShortcut = Qt::CTRL | Qt::Key_Left;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("seekToWatchLaterPositionAction");
    action.text = i18nc("@action", "Seek To Watch Later Position");
    action.iconName = QStringLiteral("pin");
    action.defaultShortcut = Qt::CTRL | Qt::SHIFT | Qt::Key_P;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("setLoopAction");
    action.text = i18nc("@action", "Set Loop");
    action.iconName = QString();
    action.defaultShortcut = Qt::Key_L;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("screenshotAction");
    action.text = i18nc("@action", "Screenshot");
    action.iconName = QStringLiteral("image-x-generic");
    action.defaultShortcut = Qt::Key_S;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("subtitleToggleAction");
    action.text = i18nc("@action", "Subtitle Toggle");
    action.iconName = QString();
    action.defaultShortcut = Qt::CTRL | Qt::Key_S;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("subtitleIncreaseFontSizeAction");
    action.text = i18nc("@action", "Subtitle Increase Font Size");
    action.iconName = QString();
    action.defaultShortcut = Qt::CTRL | Qt::Key_Z;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("subtitleDecreaseFontSizeAction");
    action.text = i18nc("@action", "Subtitle Decrease Font Size");
    action.iconName = QString();
    action.defaultShortcut = Qt::CTRL | Qt::Key_X;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("subtitleMoveUpAction");
    action.text = i18nc("@action", "Subtitle Move Up");
    action.iconName = QString();
    action.defaultShortcut = Qt::Key_R;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("subtitleMoveDownAction");
    action.text = i18nc("@action", "Subtitle Move Down");
    action.iconName = QString();
    action.defaultShortcut = Qt::SHIFT | Qt::Key_R;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("subtitleQuickenAction");
    action.text = i18nc("@action", "Subtitle Quicken");
    action.iconName = QString();
    action.defaultShortcut = Qt::Key_Z;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("subtitleDelayAction");
    action.text = i18nc("@action", "Subtitle Delay");
    action.iconName = QString();
    action.defaultShortcut = Qt::SHIFT | Qt::Key_Z;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("subtitleCycleUpAction");
    action.text = i18nc("@action", "Subtitle Cycle Up");
    action.iconName = QString();
    action.defaultShortcut = Qt::Key_J;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("subtitleCycleDownAction");
    action.text = i18nc("@action", "Subtitle Cycle Down");
    action.iconName = QString();
    action.defaultShortcut = Qt::SHIFT | Qt::Key_J;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("toggleDeinterlacingAction");
    action.text = i18nc("@action", "Toggle Deinterlacing");
    action.iconName = QString();
    action.defaultShortcut = Qt::Key_D;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("toggleFullscreenAction");
    action.text = i18nc("@action", "Toggle Fullscreen");
    action.iconName = QStringLiteral("view-fullscreen");
    action.defaultShortcut = Qt::Key_F;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("toggleMenuBarAction");
    action.text = i18nc("@action", "Toggle Menu Bar");
    action.iconName = QString();
    action.defaultShortcut = Qt::CTRL | Qt::Key_M;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("toggleHeaderAction");
    action.text = i18nc("@action", "Toggle Header");
    action.iconName = QString();
    action.defaultShortcut = Qt::CTRL | Qt::Key_H;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("togglePlaylistAction");
    action.text = i18nc("@action", "Playlist");
    action.iconName = QStringLiteral("view-media-playlist");
    action.defaultShortcut = Qt::Key_P;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("videoPanXLeftAction");
    action.text = i18nc("@action", "Video Move Left");
    action.iconName = QString();
    action.defaultShortcut = Qt::CTRL | Qt::ALT | Qt::Key_Left;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("videoPanXRightAction");
    action.text = i18nc("@action", "Video Move Right");
    action.iconName = QString();
    action.defaultShortcut = Qt::CTRL | Qt::ALT | Qt::Key_Right;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("videoPanYUpAction");
    action.text = i18nc("@action", "Video Move Up");
    action.iconName = QString();
    action.defaultShortcut = Qt::CTRL | Qt::ALT | Qt::Key_Up;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("videoPanYDownAction");
    action.text = i18nc("@action", "Video Move Down");
    action.iconName = QString();
    action.defaultShortcut = Qt::CTRL | Qt::ALT | Qt::Key_Down;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("volumeUpAction");
    action.text = i18nc("@action", "Volume Up");
    action.iconName = QStringLiteral("audio-volume-high");
    action.defaultShortcut = Qt::Key_9;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("volumeDownAction");
    action.text = i18nc("@action", "Volume Down");
    action.iconName = QStringLiteral("audio-volume-low");
    action.defaultShortcut = Qt::Key_0;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("zoomInAction");
    action.text = i18nc("@action", "Zoom In");
    action.iconName = QStringLiteral("zoom-in");
    action.defaultShortcut = Qt::CTRL | Qt::Key_Plus;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("zoomOutAction");
    action.text = i18nc("@action", "Zoom Out");
    action.iconName = QStringLiteral("zoom-out");
    action.defaultShortcut = Qt::CTRL | Qt::Key_Minus;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("zoomResetAction");
    action.text = i18nc("@action", "Zoom Reset");
    action.iconName = QStringLiteral("zoom-original");
    action.defaultShortcut = Qt::CTRL | Qt::Key_0;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("contrastUpAction");
    action.text = i18nc("@action", "Contrast Up");
    action.iconName = QStringLiteral("contrast");
    action.defaultShortcut = Qt::Key_1;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("contrastDownAction");
    action.text = i18nc("@action", "Contrast Down");
    action.iconName = QStringLiteral("contrast");
    action.defaultShortcut = Qt::Key_2;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("contrastResetAction");
    action.text = i18nc("@action", "Contrast Reset");
    action.iconName = QStringLiteral("contrast");
    action.defaultShortcut = Qt::CTRL | Qt::Key_1;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("brightnessUpAction");
    action.text = i18nc("@action", "Brightness Up");
    action.iconName = QStringLiteral("contrast");
    action.defaultShortcut = Qt::Key_3;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("brightnessDownAction");
    action.text = i18nc("@action", "Brightness Down");
    action.iconName = QStringLiteral("contrast");
    action.defaultShortcut = Qt::Key_4;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("brightnessResetAction");
    action.text = i18nc("@action", "Brightness Reset");
    action.iconName = QStringLiteral("contrast");
    action.defaultShortcut = Qt::CTRL | Qt::Key_3;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("gammaUpAction");
    action.text = i18nc("@action", "Gamma Up");
    action.iconName = QStringLiteral("contrast");
    action.defaultShortcut = Qt::Key_5;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("gammaDownAction");
    action.text = i18nc("@action", "Gamma Down");
    action.iconName = QStringLiteral("contrast");
    action.defaultShortcut = Qt::Key_6;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("gammaResetAction");
    action.text = i18nc("@action", "Gamma Reset");
    action.iconName = QStringLiteral("contrast");
    action.defaultShortcut = Qt::CTRL | Qt::Key_5;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("saturationUpAction");
    action.text = i18nc("@action", "Saturation Up");
    action.iconName = QStringLiteral("contrast");
    action.defaultShortcut = Qt::Key_7;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("saturationDownAction");
    action.text = i18nc("@action", "Saturation Down");
    action.iconName = QStringLiteral("contrast");
    action.defaultShortcut = Qt::Key_8;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("saturationResetAction");
    action.text = i18nc("@action", "Saturation Reset");
    action.iconName = QStringLiteral("contrast");
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
        return QVariant(action.shortcut);
    case DefaultShortcutRole:
        return QVariant(action.defaultShortcut);
    case DescriptionRole:
        return QVariant(action.description);
    case TypeRole:
        return QVariant(action.type);
    }

    return QVariant();
}

QHash<int, QByteArray> ActionsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[TextRole] = "text";
    roles[IconRole] = "icon";
    roles[ShortcutRole] = "shortcut";
    roles[DefaultShortcutRole] = "defaultShortcut";
    roles[DescriptionRole] = "description";
    roles[TypeRole] = "type";
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

bool ActionsModel::saveShortcut(const QString &name, const QVariant &shortcut)
{
    for (int i{0}; i < m_actions.count(); ++i) {
        if (m_actions[i].name == name) {
            return saveShortcut(i, shortcut);
        }
    }
    return false;
}

bool ActionsModel::saveShortcut(int row, const QVariant &shortcut)
{
    auto group = m_config->group(QStringLiteral("Shortcuts"));
    auto shortcutString = shortcut.value<QKeySequence>().toString(QKeySequence::PortableText);
    // action whose shortcut is changed
    auto action = &m_actions[row];

    // if shortcut is being cleared, no need to search for a conflict
    if (!shortcut.toString().isEmpty()) {
        // if shortcut is used, this is the action holding the shortcut
        Action *result = nullptr;
        int i{0};
        for (; i < m_actions.count(); ++i) {
            if (m_actions[i].shortcut == shortcutString) {
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
                result->shortcut = QString();
                group.writeEntry(result->name, result->shortcut);
                Q_EMIT shortcutChanged(result->name, result->shortcut);
                Q_EMIT dataChanged(index(i, 0), index(i, 0));
            } else {
                // user chose cancel
                return false;
            }
        }
    }
    // set shortcut on the action being changed
    action->shortcut = shortcutString;
    group.writeEntry(action->name, action->shortcut);
    Q_EMIT shortcutChanged(action->name, action->shortcut);
    Q_EMIT dataChanged(index(row, 0), index(row, 0));

    return group.sync();
}

QString ActionsModel::getShortcut(const QString &key, const QKeySequence &defaultValue) const
{
    auto v = defaultValue.toString(QKeySequence::PortableText);
    auto group = m_config->group(QStringLiteral("Shortcuts"));
    return group.readEntry(key, v);
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

    nameRegExp.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    typeRegExp.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
}

bool ProxyActionsModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex nameIndex = sourceModel()->index(sourceRow, 0, sourceParent);
    QModelIndex typeIndex = sourceModel()->index(sourceRow, 0, sourceParent);

    QString name = sourceModel()->data(nameIndex, ActionsModel::NameRole).toString();
    QString type = sourceModel()->data(typeIndex, ActionsModel::TypeRole).toString();

    return (name.contains(nameRegExp) && type.contains(typeRegExp));
}

void ProxyActionsModel::setNameFilter(const QString &regExp)
{
    nameRegExp.setPattern(regExp);
    invalidateFilter();
}

void ProxyActionsModel::setTypeFilter(const QString &regExp)
{
    typeRegExp.setPattern(regExp);
    invalidateFilter();
}

bool ProxyActionsModel::saveShortcut(int row, const QVariant &shortcut)
{
    auto actionsModel = qobject_cast<ActionsModel *>(sourceModel());
    return actionsModel->saveShortcut(mapToSource(index(row, 0)).row(), shortcut);
}

#include "moc_actionsmodel.cpp"
