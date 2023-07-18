/*
 * SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef MPVCONTROLLER_H
#define MPVCONTROLLER_H

#include <QMap>
#include <QObject>

#include <mpv/client.h>
#include <mpv/render_gl.h>

/**
 * RAII wrapper that calls mpv_free_node_contents() on the pointer.
 */
struct node_autofree {
    mpv_node *ptr;
    explicit node_autofree(mpv_node *a_ptr)
        : ptr(a_ptr)
    {
    }
    ~node_autofree()
    {
        mpv_free_node_contents(ptr);
    }
};

/**
 * This is used to return error codes wrapped in QVariant for functions which
 * return QVariant.
 *
 * You can use get_error() or is_error() to extract the error status from a
 * QVariant value.
 */
struct ErrorReturn {
    /**
     * enum mpv_error value (or a value outside of it if ABI was extended)
     */
    int error;

    ErrorReturn()
        : error(0)
    {
    }
    explicit ErrorReturn(int err)
        : error(err)
    {
    }
};
Q_DECLARE_METATYPE(ErrorReturn)

class MpvController : public QObject
{
    Q_OBJECT
public:
    MpvController(QObject *parent = nullptr);

    enum Properties {
        Pause,
        Volume,
        Position,
        Duration,
        Remaining,
        Mute,
        Chapter,
        ChapterList,
        TrackList,
        MediaTitle,
        AudioId,
        SubtitleId,
        SecondarySubtitleId,
        VideoId,
    };

    /**
     * Set the given property as mpv_node converted from the QVariant argument.
     *
     * @return mpv error code (<0 on error, >= 0 on success)
     */
    Q_INVOKABLE int setProperty(const QString &name, const QVariant &value);
    Q_INVOKABLE int setProperty(Properties property, const QVariant &value);
    Q_INVOKABLE int setPropertyAsync(Properties property, const QVariant &value, int id);

    /**
     * Return the given property as mpv_node converted to QVariant,
     * or QVariant() on error.
     *
     * @param `name` the property name
     * @return the property value, or an ErrorReturn with the error code
     */
    Q_INVOKABLE QVariant getProperty(const QString &name);
    Q_INVOKABLE int getPropertyAsync(Properties property, int id);

    /**
     * mpv_command_node() equivalent.
     *
     * @param `args` command arguments, with args[0] being the command name as string
     * @return the property value, or an ErrorReturn with the error code
     */
    Q_INVOKABLE QVariant command(const QVariant &params);

    /**
     * Return an error string from an ErrorReturn.
     */
    Q_INVOKABLE QString getError(int error);

    static void mpvEvents(void *ctx);
    void eventHandler();
    mpv_handle *mpv() const;

Q_SIGNALS:
    void propertyChanged(const QString &property, const QVariant &value);
    void getPropertyReply(const QVariant &value, int id);
    void setPropertyReply(int id);
    void fileStarted();
    void fileLoaded();
    void endFile(QString reason);
    void videoReconfig();

private:
    mpv_node_list *create_list(mpv_node *dst, bool is_map, int num);
    void setNode(mpv_node *dst, const QVariant &src);
    bool test_type(const QVariant &v, QMetaType::Type t);
    void free_node(mpv_node *dst);
    QVariant node_to_variant(const mpv_node *node);

    mpv_handle *m_mpv{nullptr};
    // clang-format off
    QMap<Properties, QString> m_properties = {
        {Pause,               QStringLiteral("pause")},
        {Volume,              QStringLiteral("volume")},
        {Position,            QStringLiteral("time-pos")},
        {Duration,            QStringLiteral("duration")},
        {Remaining,           QStringLiteral("time-remaining")},
        {Mute,                QStringLiteral("mute")},
        {Chapter,             QStringLiteral("chapter")},
        {ChapterList,         QStringLiteral("chapter-list")},
        {TrackList,           QStringLiteral("track-list")},
        {MediaTitle,          QStringLiteral("media-title")},
        {AudioId,             QStringLiteral("aid")},
        {SubtitleId,          QStringLiteral("sid")},
        {SecondarySubtitleId, QStringLiteral("secondary-sid")},
        {VideoId,             QStringLiteral("vid")},
    };
    // clang-format on
};

#endif // MPVCONTROLLER_H
