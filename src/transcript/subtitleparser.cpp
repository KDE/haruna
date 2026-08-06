/*
 * SPDX-FileCopyrightText: 2026 Muhammet Sadık Uğursoy <sadikugursoy@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "subtitleparser.h"

#include <QRegularExpression>
#include <QString>
#include <QUrl>

#include "miscutils.h"
#include "subtitleline.h"
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

using namespace Qt::StringLiterals;

SubtitleParser::SubtitleParser(QObject *parent)
    : QObject{parent}
{
}

void SubtitleParser::parseSubtitle(const QUrl &url, const int streamIndex, const int transcriptModelVersion, const std::atomic<bool> &cancelRequested)
{
    if (!url.isLocalFile()) {
        return;
    }

    QString path = url.toLocalFile();

    AVFormatContext *fmt_ctx = avformat_alloc_context();
    if (!fmt_ctx) {
        return;
    }

    if (avformat_open_input(&fmt_ctx, path.toUtf8().data(), nullptr, nullptr) < 0) {
        avformat_free_context(fmt_ctx);
        return;
    }

    if (avformat_find_stream_info(fmt_ctx, nullptr) < 0) {
        avformat_close_input(&fmt_ctx);
        return;
    }

    if (cancelRequested) {
        avformat_close_input(&fmt_ctx);
        return;
    }

    // Initialize the subtitle decoder
    const AVCodecParameters *codecpar = fmt_ctx->streams[streamIndex]->codecpar;
    const AVCodec *decoder = avcodec_find_decoder(codecpar->codec_id);
    if (!decoder) {
        avformat_close_input(&fmt_ctx);
        return;
    }

    AVCodecContext *codecContext = avcodec_alloc_context3(decoder);
    if (!codecContext) {
        avformat_close_input(&fmt_ctx);
        return;
    }

    if (avcodec_parameters_to_context(codecContext, codecpar) < 0) {
        avcodec_free_context(&codecContext);
        avformat_close_input(&fmt_ctx);
        return;
    }

    if (avcodec_open2(codecContext, decoder, nullptr) < 0) {
        avcodec_free_context(&codecContext);
        avformat_close_input(&fmt_ctx);
        return;
    }

    // Read the subtitle data from the codex context
    AVPacket *packet = av_packet_alloc();
    if (!packet) {
        avcodec_free_context(&codecContext);
        avformat_close_input(&fmt_ctx);
        return;
    }

    AVSubtitle subtitle;
    const AVRational timeBase = fmt_ctx->streams[streamIndex]->time_base;
    double timeMultiplier = 1.0 * timeBase.num / timeBase.den;
    int gotSub = 0;
    while (av_read_frame(fmt_ctx, packet) >= 0) {
        if (cancelRequested) {
            av_packet_unref(packet);
            av_packet_free(&packet);
            avcodec_free_context(&codecContext);
            avformat_close_input(&fmt_ctx);
            return;
        }

        if (packet->stream_index == int(streamIndex)) {
            // Decode the subtitle packet
            int ret = avcodec_decode_subtitle2(codecContext, &subtitle, &gotSub, packet);

            if (ret >= 0 && gotSub != 0) {
                int64_t startSecs = packet->pts;
                int64_t duration = packet->duration;
                int64_t endSecs = startSecs + duration;

                QString text;
                QString line;

                for (unsigned int i = 0; i < subtitle.num_rects; i++) {
                    // Note: Even though SUBTITLE_ASS sounds like they are for ASS/SSA only, that is not true.
                    // Most modern subtitles are handled as SUBTITLE_ASS (srt, sub, ssa, ass, smi) by FFMpeg internally.
                    // SUBTITLE_TEXT is only used by very simple legacy formats that have no styling capabilities.
                    switch (subtitle.rects[i]->type) {
                    case SUBTITLE_TEXT: {
                        line = QString::fromUtf8(subtitle.rects[i]->text);
                        break;
                    }
                    case SUBTITLE_ASS: {
                        line = QString::fromUtf8(subtitle.rects[i]->ass);
                        line = formatASS(line);
                        break;
                    }
                    case SUBTITLE_BITMAP:
                    case SUBTITLE_NONE: {
                        break;
                    }
                    }

                    if (line.isEmpty()) {
                        continue;
                    }

                    text.append(line);
                }

                if (text.size() == 0) {
                    avsubtitle_free(&subtitle);
                    av_packet_unref(packet);
                    continue;
                }

                SubtitleLine transcriptItem;
                transcriptItem.text = text;
                transcriptItem.duration = duration;
                transcriptItem.startTime = startSecs;
                transcriptItem.endTime = endSecs;
                transcriptItem.formattedStartTime = MiscUtils::formatTime(startSecs * timeMultiplier);
                transcriptItem.formattedEndTime = MiscUtils::formatTime(endSecs * timeMultiplier);
                Q_EMIT transcriptItemReady(transcriptItem, transcriptModelVersion);

                // must be freed when gotSub is set
                avsubtitle_free(&subtitle);
            }
        }
        av_packet_unref(packet);
    }

    av_packet_free(&packet);
    avcodec_free_context(&codecContext);
    avformat_close_input(&fmt_ctx);
}

QString SubtitleParser::formatASS(const QString in)
{
    // libavc returns the parsed text as a comma separated line, consists of 8 headers and lastly, the text
    // The text is always guaranteed to be the last one since it can contain commas.
    // We need to skip the first 8 columns and join the rest.
    QString text = in.section(QChar::fromLatin1(','), 8);

    // Remove background drawings. They are sandwiched between \pn and \p0 where n>0 and is a scaling factor. p tag can also be inside any other number of other
    // tags. Text can also be appended or prepended before the background drawing, therefore we need to carve out the drawing part. If p0 tag does not exist,
    // the whole line is a drawing and should be discarded.
    QRegularExpression drawing(u"\\{[^}]*\\\\p[1-9][^}]*\\}[^{]*(\\{[^}]*\\\\p0\\})*"_s);
    while (drawing.match(text).hasMatch()) {
        text.remove(drawing);
    }

    if (text.isEmpty()) {
        return QString::fromUtf8(text.toUtf8());
    }

    // Remove markup text
    QRegularExpression re(u"\\{[^}]*\\}"_s);
    while (re.match(text).hasMatch()) {
        text.remove(re);
    }
    // FFMpeg decoder adds \\N as newline for ASS type.
    text.replace(u"\\N"_s, u"\n"_s, Qt::CaseSensitive);

    return QString::fromUtf8(text.toUtf8());
}
