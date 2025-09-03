/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef MPVOBJECT_H
#define MPVOBJECT_H

#include <QtQuick/QQuickFramebufferObject>

#include <mpv/client.h>
#include <mpv/render_gl.h>

class MpvRenderer;
class Track;
class TracksModel;

class MpvObject : public QQuickFramebufferObject
{
    Q_OBJECT
    Q_PROPERTY(double position  MEMBER m_position   NOTIFY onPositionChanged)
    Q_PROPERTY(double duration  MEMBER m_duration   NOTIFY onDurationChanged)
    Q_PROPERTY(double remaining MEMBER m_remaining  NOTIFY onRemainingChanged)
    Q_PROPERTY(double volume    MEMBER m_volume     NOTIFY onVolumeChanged)
    Q_PROPERTY(bool pause       MEMBER m_pause      NOTIFY onPauseChanged)
    Q_PROPERTY(int contrast     MEMBER m_contrast   NOTIFY onContrastChanged)
    Q_PROPERTY(int brightness   MEMBER m_brightness NOTIFY onBrightnessChanged)
    Q_PROPERTY(int gamma        MEMBER m_gamma      NOTIFY onGammaChanged)
    Q_PROPERTY(int saturation   MEMBER m_saturation NOTIFY onSaturationChanged)
    Q_PROPERTY(int chapter      MEMBER m_chapter    NOTIFY onChapterChanged)

    mpv_handle *mpv;
    mpv_render_context *mpv_gl;

    friend class MpvRenderer;
    void handle_mpv_event(mpv_event *event);
public:
    static void on_update(void *ctx);

    MpvObject(QQuickItem * parent = 0);
    virtual ~MpvObject();
    virtual Renderer *createRenderer() const;

public slots:
    void eventHandler();
    static void on_mpv_events(void *ctx);
    QVariant command(const QVariant& params);
    int setProperty(const QString& name, const QVariant& value);
    QVariant getProperty(const QString &name);
    QString formatTime(double time);
    TracksModel *audioTracksModel() const;
    TracksModel *subtitleTracksModel() const;

signals:
    void onUpdate();
    void onPositionChanged(double);
    void onDurationChanged(double);
    void onRemainingChanged(double);
    void onVolumeChanged(double);
    void onPauseChanged(bool);
    void onChapterChanged(int);
    void onContrastChanged(int);
    void onBrightnessChanged(int);
    void onGammaChanged(int);
    void onSaturationChanged(int);
    void fileLoaded();
    void endOfFile();
    void ready();

private slots:
    void doUpdate();
private:
    TracksModel *m_audioTracksModel;
    TracksModel *m_subtitleTracksModel;
    QMap<int, Track*> m_subtitleTracks;
    QMap<int, Track*> m_audioTracks;
    double m_position;
    double m_duration;
    double m_remaining;
    double m_volume;
    QVariant m_chapters;
    int m_chapter;
    bool m_pause;
    int m_contrast = 0;
    int m_brightness = 0;
    int m_gamma = 0;
    int m_saturation = 0;

    void loadTracks();
};

class MpvRenderer : public QQuickFramebufferObject::Renderer
{
public:
    MpvRenderer(MpvObject *new_obj);
    ~MpvRenderer();

    MpvObject *obj;

    // This function is called when a new FBO is needed.
    // This happens on the initial frame.
    QOpenGLFramebufferObject * createFramebufferObject(const QSize &size);

    void render();
};

#endif // MPVOBJECT_H
