/*
 * SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef MPVPREVIEW_H
#define MPVPREVIEW_H

#include <MpvAbstractItem>

class MpvRenderer;

class MpvPreview : public MpvAbstractItem
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(double position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(double aspectRatio READ aspectRatio NOTIFY aspectRatioChanged)
    Q_PROPERTY(QString file READ file WRITE setFile NOTIFY fileChanged)
    Q_PROPERTY(bool accuratePreview READ accuratePreview WRITE setAccuratePreview NOTIFY accuratePreviewChanged)
    Q_PROPERTY(bool isLocalFile READ isLocalFile WRITE setIsLocalFile NOTIFY isLocalFileChanged)
    Q_PROPERTY(bool isVideo READ isVideo WRITE setIsVideo NOTIFY isVideoChanged)

public:
    MpvPreview();

    double position();
    void setPosition(double value);

    double aspectRatio();

    QString file() const;
    void setFile(const QString &_file);

    Q_INVOKABLE void loadFile();

    bool accuratePreview() const;
    void setAccuratePreview(bool _accuratePreview);

    bool isLocalFile() const;
    void setIsLocalFile(bool _isLocalFile);

    bool isVideo() const;
    void setIsVideo(bool _isVideo);

Q_SIGNALS:
    void positionChanged();
    void aspectRatioChanged();
    void fileChanged();
    void accuratePreviewChanged();
    void isLocalFileChanged();

    void isVideoChanged();

private:
    double m_position{0.0};
    QString m_file;
    bool m_accuratePreview{false};
    bool m_isLocalFile{false};
    bool m_isReady{false};
    bool m_isVideo{false};
};

#endif // MPVPREVIEW_H
