/*
    SPDX-FileCopyrightText: 2010 Dirk Vanden Boer <dirk.vdb@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef MOVIEDECODER_H
#define MOVIEDECODER_H

#include <QString>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavfilter/avfilter.h>
#include <libavformat/avformat.h>
}

class QImage;

class FrameDecoder
{
public:
    explicit FrameDecoder(const QString &filename, AVFormatContext *pavContext = nullptr);
    ~FrameDecoder();

    QString getCodec();
    void seek(int timeInSeconds);
    bool decodeVideoFrame();
    void getScaledVideoFrame(int scaledSize, bool maintainAspectRatio, QImage &videoFrame);

    int getWidth();
    int getHeight();
    int getDuration();

    void initialize(const QString &filename);
    void destroy();
    bool getInitialized();

private:
    bool initializeVideo();

    bool decodeVideoPacket();
    bool getVideoPacket();
    void convertAndScaleFrame(AVPixelFormat format, int scaledSize, bool maintainAspectRatio, int &scaledWidth, int &scaledHeight);
    void createAVFrame(AVFrame **avFrame, quint8 **frameBuffer, int width, int height, AVPixelFormat format);
    void calculateDimensions(int squareSize, bool maintainAspectRatio, int &destWidth, int &destHeight);

    void deleteFilterGraph();
    bool initFilterGraph(enum AVPixelFormat pixfmt, int width, int height);
    bool processFilterGraph(AVFrame *dst, const AVFrame *src, enum AVPixelFormat pixfmt, int width, int height);

private:
    int m_VideoStream;
    AVFormatContext *m_pFormatContext{nullptr};
    AVCodecContext *m_pVideoCodecContext{nullptr};
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(59, 0, 100)
    AVCodec *m_pVideoCodec{nullptr};
#else
    const AVCodec *m_pVideoCodec{nullptr};
#endif
    AVFrame *m_pFrame{nullptr};
    quint8 *m_pFrameBuffer{nullptr};
    AVPacket *m_pPacket{nullptr};
    bool m_FormatContextWasGiven;
    bool m_AllowSeek;
    bool m_initialized;
    AVFilterContext *m_bufferSinkContext{nullptr};
    AVFilterContext *m_bufferSourceContext{nullptr};
    AVFilterGraph *m_filterGraph{nullptr};
    AVFrame *m_filterFrame{nullptr};
    int m_lastWidth;
    int m_lastHeight;
    enum AVPixelFormat m_lastPixfmt;
};

#endif
