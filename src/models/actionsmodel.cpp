/*
 * SPDX-FileCopyrightText: 2022 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "actionsmodel.h"

#include <global.h>

#include <KConfigGroup>

#include <QMessageBox>
#include <QPushButton>

ActionsModel::ActionsModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_config = KSharedConfig::openConfig(Global::instance()->appConfigFilePath());

    Action action;
    action.name = QStringLiteral("aboutHarunaAction");
    action.text = i18n("About Haruna");
    action.iconName = QStringLiteral("haruna");
    action.defaultShortcut = Qt::Key_F1;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("audioCycleUpAction");
    action.text = i18n("Audio Cycle Up");
    action.iconName = QStringLiteral("");
    action.defaultShortcut = Qt::SHIFT + Qt::Key_3;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("audioCycleDownAction");
    action.text = i18n("Audio Cycle Down");
    action.iconName = QStringLiteral("");
    action.defaultShortcut = Qt::SHIFT + Qt::Key_2;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("configureAction");
    action.text = i18n("Configure");
    action.iconName = QStringLiteral("configure");
    action.defaultShortcut = Qt::CTRL + Qt::SHIFT + Qt::Key_Comma;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("configureShortcutsAction");
    action.text = i18n("Configure Keyboard Shortcuts");
    action.iconName = QStringLiteral("configure-shortcuts");
    action.defaultShortcut = Qt::CTRL + Qt::ALT + Qt::Key_Comma;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("exitFullscreenAction");
    action.text = i18n("Exit Fullscreen");
    action.iconName = QStringLiteral("");
    action.defaultShortcut = Qt::Key_Escape;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("frameStepForwardAction");
    action.text = i18n("Frame Step Forward");
    action.iconName = QStringLiteral("");
    action.defaultShortcut = Qt::Key_Period;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QStringLiteral("Move one frame forward, then pause");
    m_actions << action;

    action.name = QStringLiteral("frameStepBackwardAction");
    action.text = i18n("Frame Step Backward");
    action.iconName = QStringLiteral("");
    action.defaultShortcut = Qt::Key_Comma;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = QStringLiteral("Move one frame backward, then pause");
    m_actions << action;

    action.name = QStringLiteral("loadLastPlayedFileAction");
    action.text = i18n("Load Last Played File");
    action.iconName = QStringLiteral("document-open-recent");
    action.defaultShortcut = Qt::CTRL + Qt::SHIFT + Qt::Key_L;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("muteAction");
    action.text = i18n("Mute");
    action.iconName = QStringLiteral("player-volume");
    action.defaultShortcut = Qt::Key_M;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("openContextMenuAction");
    action.text = i18n("Open Context Menu");
    action.iconName = QStringLiteral("application-menu");
    action.defaultShortcut = Qt::Key_Menu;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    action.description = i18n("Opens and closes the context menu at the position of the mouse pointer");
    m_actions << action;

    action.name = QStringLiteral("openFileAction");
    action.text = i18n("Open File");
    action.iconName = QStringLiteral("folder-videos");
    action.defaultShortcut = Qt::CTRL + Qt::Key_O;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("openUrlAction");
    action.text = i18n("Open Url");
    action.iconName = QStringLiteral("internet-services");
    action.defaultShortcut = Qt::CTRL + Qt::SHIFT + Qt::Key_O;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("playbackSpeedIncreaseAction");
    action.text = i18n("Playback speed increase");
    action.iconName = QStringLiteral("");
    action.defaultShortcut = Qt::Key_BracketRight;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("playbackSpeedDecreaseAction");
    action.text = i18n("Playback speed decrease");
    action.iconName = QStringLiteral("");
    action.defaultShortcut = Qt::Key_BracketLeft;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("playbackSpeedResetAction");
    action.text = i18n("Playback speed reset");
    action.iconName = QStringLiteral("");
    action.defaultShortcut = Qt::Key_Backspace;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("playPauseAction");
    action.text = i18n("Play/Pause");
    action.iconName = QStringLiteral("media-playback-pause");
    action.defaultShortcut = Qt::Key_Space;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("playNextAction");
    action.text = i18n("Play Next");
    action.iconName = QStringLiteral("media-skip-forward");
    action.defaultShortcut = Qt::SHIFT + Qt::Key_Period;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("playPreviousAction");
    action.text = i18n("Play Previous");
    action.iconName = QStringLiteral("media-skip-backward");
    action.defaultShortcut = Qt::SHIFT + Qt::Key_Comma;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("quitApplicationAction");
    action.text = i18n("Quit");
    action.iconName = QStringLiteral("application-exit");
    action.defaultShortcut = Qt::CTRL + Qt::Key_Q;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("restartPlaybackAction");
    action.text = i18n("Restart Playback");
    action.iconName = QStringLiteral("start-over");
    action.defaultShortcut = Qt::Key_F5;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("seekForwardSmallAction");
    action.text = i18n("Seek Small Step Forward");
    action.iconName = QStringLiteral("media-seek-forward");
    action.defaultShortcut = Qt::Key_Right;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("seekBackwardSmallAction");
    action.text = i18n("Seek Small Step Backward");
    action.iconName = QStringLiteral("media-seek-backward");
    action.defaultShortcut = Qt::Key_Left;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("seekForwardMediumAction");
    action.text = i18n("Seek Medium Step Forward");
    action.iconName = QStringLiteral("media-seek-forward");
    action.defaultShortcut = Qt::SHIFT + Qt::Key_Right;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("seekBackwardMediumAction");
    action.text = i18n("Seek Medium Step Backward");
    action.iconName = QStringLiteral("media-seek-backward");
    action.defaultShortcut = Qt::SHIFT + Qt::Key_Left;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("seekForwardBigAction");
    action.text = i18n("Seek Big Step Forward");
    action.iconName = QStringLiteral("media-seek-forward");
    action.defaultShortcut = Qt::Key_Up;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("seekBackwardBigAction");
    action.text = i18n("Seek Big Step Backward");
    action.iconName = QStringLiteral("media-seek-backward");
    action.defaultShortcut = Qt::Key_Down;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("seekNextChapterAction");
    action.text = i18n("Seek Next Chapter");
    action.iconName = QStringLiteral("media-seek-forward");
    action.defaultShortcut = Qt::Key_PageUp;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("seekPreviousChapterAction");
    action.text = i18n("Seek Previous Chapter");
    action.iconName = QStringLiteral("media-seek-backward");
    action.defaultShortcut = Qt::Key_PageDown;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("seekNextSubtitleAction");
    action.text = i18n("Seek To Next Subtitle");
    action.iconName = QStringLiteral("media-seek-forward");
    action.defaultShortcut = Qt::CTRL + Qt::Key_Right;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("seekPreviousSubtitleAction");
    action.text = i18n("Seek To Previous Subtitle");
    action.iconName = QStringLiteral("media-seek-backward");
    action.defaultShortcut = Qt::CTRL + Qt::Key_Left;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("seekToWatchLaterPositionAction");
    action.text = i18n("Seek To Watch Later Position");
    action.iconName = QStringLiteral("pin");
    action.defaultShortcut = Qt::CTRL + Qt::SHIFT + Qt::Key_P;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("setLoopAction");
    action.text = i18n("Set Loop");
    action.iconName = QStringLiteral("");
    action.defaultShortcut = Qt::Key_L;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("screenshotAction");
    action.text = i18n("Screenshot");
    action.iconName = QStringLiteral("image-x-generic");
    action.defaultShortcut = Qt::Key_S;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("subtitleToggleAction");
    action.text = i18n("Subtitle Toggle");
    action.iconName = QStringLiteral("");
    action.defaultShortcut = Qt::CTRL + Qt::Key_S;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("subtitleIncreaseFontSizeAction");
    action.text = i18n("Subtitle Increase Font Size");
    action.iconName = QStringLiteral("");
    action.defaultShortcut = Qt::CTRL + Qt::Key_Z;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("subtitleDecreaseFontSizeAction");
    action.text = i18n("Subtitle Decrease Font Size");
    action.iconName = QStringLiteral("");
    action.defaultShortcut = Qt::CTRL + Qt::Key_X;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("subtitleMoveUpAction");
    action.text = i18n("Subtitle Move Up");
    action.iconName = QStringLiteral("");
    action.defaultShortcut = Qt::Key_R;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("subtitleMoveDownAction");
    action.text = i18n("Subtitle Move Down");
    action.iconName = QStringLiteral("");
    action.defaultShortcut = Qt::SHIFT + Qt::Key_R;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("subtitleQuickenAction");
    action.text = i18n("Subtitle Quicken");
    action.iconName = QStringLiteral("");
    action.defaultShortcut = Qt::Key_Z;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("subtitleDelayAction");
    action.text = i18n("Subtitle Delay");
    action.iconName = QStringLiteral("");
    action.defaultShortcut = Qt::SHIFT + Qt::Key_Z;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("subtitleCycleUpAction");
    action.text = i18n("Subtitle Cycle Up");
    action.iconName = QStringLiteral("");
    action.defaultShortcut = Qt::Key_J;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("subtitleCycleDownAction");
    action.text = i18n("Subtitle Cycle Down");
    action.iconName = QStringLiteral("");
    action.defaultShortcut = Qt::SHIFT + Qt::Key_J;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("toggleDeinterlacingAction");
    action.text = i18n("Toggle Deinterlacing");
    action.iconName = QStringLiteral("");
    action.defaultShortcut = Qt::Key_D;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("toggleFullscreenAction");
    action.text = i18n("Toggle Fullscreen");
    action.iconName = QStringLiteral("view-fullscreen");
    action.defaultShortcut = Qt::Key_F;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("toggleMenuBarAction");
    action.text = i18n("Toggle Menu Bar");
    action.iconName = QStringLiteral("");
    action.defaultShortcut = Qt::CTRL + Qt::Key_M;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("toggleHeaderAction");
    action.text = i18n("Toggle Header");
    action.iconName = QStringLiteral("");
    action.defaultShortcut = Qt::CTRL + Qt::Key_H;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("togglePlaylistAction");
    action.text = i18n("Toggle Playlist");
    action.iconName = QStringLiteral("view-media-playlist");
    action.defaultShortcut = Qt::Key_P;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("videoPanXLeftAction");
    action.text = i18n("Video pan x left");
    action.iconName = QStringLiteral("");
    action.defaultShortcut = Qt::CTRL + Qt::ALT + Qt::Key_Left;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("videoPanXRightAction");
    action.text = i18n("Video pan x right");
    action.iconName = QStringLiteral("");
    action.defaultShortcut = Qt::CTRL + Qt::ALT + Qt::Key_Right;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("videoPanYUpAction");
    action.text = i18n("Video pan y up");
    action.iconName = QStringLiteral("");
    action.defaultShortcut = Qt::CTRL + Qt::ALT + Qt::Key_Up;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("videoPanYDownAction");
    action.text = i18n("Video pan y down");
    action.iconName = QStringLiteral("");
    action.defaultShortcut = Qt::CTRL + Qt::ALT + Qt::Key_Down;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("volumeUpAction");
    action.text = i18n("Volume Up");
    action.iconName = QStringLiteral("audio-volume-high");
    action.defaultShortcut = Qt::Key_9;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("volumeDownAction");
    action.text = i18n("Volume Down");
    action.iconName = QStringLiteral("audio-volume-low");
    action.defaultShortcut = Qt::Key_0;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("zoomInAction");
    action.text = i18n("Zoom In");
    action.iconName = QStringLiteral("zoom-in");
    action.defaultShortcut = Qt::CTRL + Qt::Key_Plus;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("zoomOutAction");
    action.text = i18n("Zoom Out");
    action.iconName = QStringLiteral("zoom-out");
    action.defaultShortcut = Qt::CTRL + Qt::Key_Minus;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("zoomResetAction");
    action.text = i18n("Zoom Reset");
    action.iconName = QStringLiteral("zoom-original");
    action.defaultShortcut = Qt::CTRL + Qt::Key_0;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;


    action.name = QStringLiteral("contrastUpAction");
    action.text = i18n("Contrast Up");
    action.iconName = QStringLiteral("contrast");
    action.defaultShortcut = Qt::Key_1;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("contrastDownAction");
    action.text = i18n("Contrast Down");
    action.iconName = QStringLiteral("contrast");
    action.defaultShortcut = Qt::Key_2;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("contrastResetAction");
    action.text = i18n("Contrast Reset");
    action.iconName = QStringLiteral("contrast");
    action.defaultShortcut = Qt::CTRL + Qt::Key_1;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("brightnessUpAction");
    action.text = i18n("Brightness Up");
    action.iconName = QStringLiteral("contrast");
    action.defaultShortcut = Qt::Key_3;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("brightnessDownAction");
    action.text = i18n("Brightness Down");
    action.iconName = QStringLiteral("contrast");
    action.defaultShortcut = Qt::Key_4;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("brightnessResetAction");
    action.text = i18n("Brightness Reset");
    action.iconName = QStringLiteral("contrast");
    action.defaultShortcut = Qt::CTRL + Qt::Key_3;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("gammaUpAction");
    action.text = i18n("Gamma Up");
    action.iconName = QStringLiteral("contrast");
    action.defaultShortcut = Qt::Key_5;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("gammaDownAction");
    action.text = i18n("Gamma Down");
    action.iconName = QStringLiteral("contrast");
    action.defaultShortcut = Qt::Key_6;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("gammaResetAction");
    action.text = i18n("Gamma Reset");
    action.iconName = QStringLiteral("contrast");
    action.defaultShortcut = Qt::CTRL + Qt::Key_5;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("saturationUpAction");
    action.text = i18n("Saturation Up");
    action.iconName = QStringLiteral("contrast");
    action.defaultShortcut = Qt::Key_7;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("saturationDownAction");
    action.text = i18n("Saturation Down");
    action.iconName = QStringLiteral("contrast");
    action.defaultShortcut = Qt::Key_8;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;

    action.name = QStringLiteral("saturationResetAction");
    action.text = i18n("Saturation Reset");
    action.iconName = QStringLiteral("contrast");
    action.defaultShortcut = Qt::CTRL + Qt::Key_7;
    action.shortcut = getShortcut(action.name, action.defaultShortcut);
    m_actions << action;
}

int ActionsModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return m_actions.count();
}

QVariant ActionsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

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
    for (int i {0}; i < m_actions.count(); ++i) {
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
    for (int i {0}; i < m_actions.count(); ++i) {
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
        int i {0};
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
                result->shortcut = "";
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
    auto message = i18n("The chosen shortcut conflicts with "
                        "the shortcut of the <b>%1</b> action.<br><br>"
                        "Do you want to reassign the shortcut?", actionText);
    QMessageBox msgBox;
    msgBox.setText(message);
    QPushButton *reassignBtn = msgBox.addButton(i18n("Reassign"), QMessageBox::AcceptRole);
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
    QMetaObject::invokeMethod(this, actionName.toUtf8());
}

ProxyActionsModel::ProxyActionsModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setDynamicSortFilter(true);
    setFilterCaseSensitivity(Qt::CaseInsensitive);

    nameRegExp.setCaseSensitivity(Qt::CaseInsensitive);
    nameRegExp.setPatternSyntax(QRegExp::RegExp);
    typeRegExp.setCaseSensitivity(Qt::CaseInsensitive);
    typeRegExp.setPatternSyntax(QRegExp::RegExp);
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
