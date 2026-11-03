/*
 * SPDX-FileCopyrightText: 2026 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef OSD_H
#define OSD_H

#include <QObject>
#include <qqmlintegration.h>

class QJSEngine;
class QQmlEngine;

struct OsdOption {
    Q_GADGET
    QML_VALUE_TYPE(osd_option)

public:
    Q_PROPERTY(QString key MEMBER key)
    QString key;
    Q_PROPERTY(QString label MEMBER label)
    QString label;
};

class OsdUtils : public QObject
{
    Q_OBJECT
    QML_SINGLETON
    QML_ELEMENT

public:
    static OsdUtils &instance();
    static OsdUtils *create(QQmlEngine *, QJSEngine *);

    enum class Context {
        None,
        MpvABLoop,
        MpvChapter,
        MpvDeinterlace,
        MpvImageAdjustments,
        MpvLoadFile,
        MpvPlaybackSpeed,
        MpvScreenshot,
        MpvSubtitleDelay,
        MpvSubtitleScale,
        MpvSubtitleToggle,
        MpvVolume,
        MpvTrack,
        MpvZoom,
    };
    Q_ENUM(Context)

    Q_INVOKABLE void message(const QString &message, Context context);
    Q_INVOKABLE QList<OsdOption> options();

Q_SIGNALS:
    void messageReceived(const QString &message, OsdUtils::Context context);

private:
    explicit OsdUtils(QObject *parent = nullptr);
    ~OsdUtils() = default;

    OsdUtils(const OsdUtils &) = delete;
    OsdUtils &operator=(const OsdUtils &) = delete;
    OsdUtils(OsdUtils &&) = delete;
    OsdUtils &operator=(OsdUtils &&) = delete;

    QString contextToString(Context context);
};

#endif // OSD_H
