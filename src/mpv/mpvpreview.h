#ifndef MPVPREVIEW_H
#define MPVPREVIEW_H

#include "mpvabstractitem.h"

class MpvRenderer;

class MpvPreview : public MpvAbstractItem
{
    Q_OBJECT
    Q_PROPERTY(double position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(QString file READ file WRITE setFile NOTIFY fileChanged)
    Q_PROPERTY(bool accuratePreview READ accuratePreview WRITE setAccuratePreview NOTIFY accuratePreviewChanged)

public:
    MpvPreview();

    double position();
    void setPosition(double value);

    QString file() const;
    void setFile(const QString &_file);

    Q_INVOKABLE void loadFile();

    bool accuratePreview() const;
    void setAccuratePreview(bool _accuratePreview);

Q_SIGNALS:
    void positionChanged();
    void fileChanged();

    void accuratePreviewChanged();

private:
    double m_position{0.0};
    QString m_file;
    bool m_accuratePreview;
};

#endif // MPVPREVIEW_H
