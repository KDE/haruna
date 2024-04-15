/*
 * SPDX-FileCopyrightText: 2022 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef ACTIONSMODEL_H
#define ACTIONSMODEL_H

#include <KLocalizedString>
#include <KSharedConfig>

#include <QAbstractListModel>
#include <QKeySequence>
#include <QQmlPropertyMap>
#include <QSortFilterProxyModel>
#include <QtQml/qqmlregistration.h>

struct Action {
    QString name;
    QString text;
    QString iconName;
    QString shortcut;
    QKeySequence defaultShortcut;
    QString description;
    QString type = QStringLiteral("NormalAction");
};

class ProxyActionsModel : public QSortFilterProxyModel
{
    Q_OBJECT
    QML_NAMED_ELEMENT(ProxyActionsModel)

public:
    explicit ProxyActionsModel(QObject *parent = nullptr);
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

public Q_SLOTS:
    void setNameFilter(const QString &regExp);
    void setTypeFilter(const QString &regExp);
    bool saveShortcut(int row, const QVariant &shortcut);

private:
    QRegularExpression nameRegExp;
    QRegularExpression typeRegExp;
};

class ActionsModel : public QAbstractListModel
{
    Q_OBJECT
    QML_NAMED_ELEMENT(ActionsModel)

public:
    explicit ActionsModel(QObject *parent = nullptr);

    enum Roles {
        NameRole = Qt::UserRole + 1,
        TextRole,
        IconRole,
        ShortcutRole,
        DefaultShortcutRole,
        DescriptionRole,
        TypeRole,
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    void appendCustomAction(const Action &action);
    void editCustomAction(const QString &name, const QString &text, const QString &description);
    Q_INVOKABLE bool saveShortcut(const QString &name, const QVariant &shortcut);
    Q_INVOKABLE bool saveShortcut(int row, const QVariant &shortcut);
    Q_INVOKABLE QString getShortcut(const QString &key, const QKeySequence &defaultValue) const;
    Q_INVOKABLE void signalEmitter(const QString &actionName);

    QList<Action> &actions();

Q_SIGNALS:
    void shortcutChanged(const QString &actionName, const QString &actionShortcut);
    void openActionsDialogAction();
    void aboutHarunaAction();
    void audioCycleUpAction();
    void audioCycleDownAction();
    void configureAction();
    void configureShortcutsAction();
    void exitFullscreenAction();
    void frameStepForwardAction();
    void frameStepBackwardAction();
    void loadLastPlayedFileAction();
    void muteAction();
    void openContextMenuAction();
    void openFileAction();
    void openSubtitlesFileAction();
    void openUrlAction();
    void playbackSpeedIncreaseAction();
    void playbackSpeedDecreaseAction();
    void playbackSpeedResetAction();
    void playPauseAction();
    void playNextAction();
    void playPreviousAction();
    void quitApplicationAction();
    void restartPlaybackAction();
    void seekForwardSmallAction();
    void seekBackwardSmallAction();
    void seekForwardMediumAction();
    void seekBackwardMediumAction();
    void seekForwardBigAction();
    void seekBackwardBigAction();
    void seekNextChapterAction();
    void seekPreviousChapterAction();
    void seekNextSubtitleAction();
    void seekPreviousSubtitleAction();
    void seekToWatchLaterPositionAction();
    void setLoopAction();
    void screenshotAction();
    void subtitleToggleAction();
    void subtitleIncreaseFontSizeAction();
    void subtitleDecreaseFontSizeAction();
    void subtitleMoveUpAction();
    void subtitleMoveDownAction();
    void subtitleQuickenAction();
    void subtitleDelayAction();
    void subtitleCycleUpAction();
    void subtitleCycleDownAction();
    void toggleDeinterlacingAction();
    void toggleFullscreenAction();
    void toggleMenuBarAction();
    void toggleHeaderAction();
    void togglePlaylistAction();
    void videoPanXLeftAction();
    void videoPanXRightAction();
    void videoPanYUpAction();
    void videoPanYDownAction();
    void volumeUpAction();
    void volumeDownAction();
    void zoomInAction();
    void zoomOutAction();
    void zoomResetAction();

    void contrastUpAction();
    void contrastDownAction();
    void contrastResetAction();
    void brightnessUpAction();
    void brightnessDownAction();
    void brightnessResetAction();
    void gammaUpAction();
    void gammaDownAction();
    void gammaResetAction();
    void saturationUpAction();
    void saturationDownAction();
    void saturationResetAction();

private:
    bool keyConflictMessageBox(const QString &actionText);
    KSharedConfig::Ptr m_config;
    QList<Action> m_actions;
};

#endif // ACTIONSMODEL_H
