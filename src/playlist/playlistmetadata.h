/*
 * SPDX-FileCopyrightText: 2025 Muhammet Sadık Uğursoy <sadikugursoy@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef PLAYLISTMETADATA_H
#define PLAYLISTMETADATA_H

#include <KFileMetaData/SimpleExtractionResult>

struct AudioMetaData {
    int trackNo{0};
    int discNo{0};
    int releaseYear{0};
    int bitrate{0};
    int sampleRate{0};
    QString genre;
    QString artist;
    QString album;
    QString albumArtist;
    QString composer;
    QString lyricist;
    QString audioCodec;

    void setMetaData(KFileMetaData::PropertyMultiMap metaData)
    {
        trackNo = metaData.value(KFileMetaData::Property::TrackNumber).toInt();
        discNo = metaData.value(KFileMetaData::Property::DiscNumber).toInt();
        releaseYear = metaData.value(KFileMetaData::Property::ReleaseYear).toInt();
        bitrate = metaData.value(KFileMetaData::Property::BitRate).toInt();
        sampleRate = metaData.value(KFileMetaData::Property::SampleRate).toInt();
        genre = metaData.value(KFileMetaData::Property::Genre).toString();
        artist = metaData.value(KFileMetaData::Property::Artist).toString();
        album = metaData.value(KFileMetaData::Property::Album).toString();
        albumArtist = metaData.value(KFileMetaData::Property::AlbumArtist).toString();
        composer = metaData.value(KFileMetaData::Property::Composer).toString();
        lyricist = metaData.value(KFileMetaData::Property::Lyricist).toString();
        audioCodec = metaData.value(KFileMetaData::Property::AudioCodec).toString();
    }
};

struct VideoMetaData {
    enum Orientation {
        Square,
        Landscape,
        Portrait,
    };

    int width{0};
    int height{0};
    int rotation{0}; // KFileMetaData does not expose rotation for now
    double aspectRatio{0.0};
    double frameRate{0.0};
    QString videoCodec;
    Orientation displayedOrientation{Orientation::Square};
    int displayWidth{0}; // width after considering rotation
    int displayHeight{0}; // height after considering rotation

    // Checks if video has rotation. Updates display width/height/orientation
    // Only considers multiples of 90 degrees (similar to VLC)
    void calculateDisplayVariables()
    {
        if (width > height) {
            if (rotation == 0 || rotation == 180) {
                displayHeight = height;
                displayWidth = width;
                displayedOrientation = Orientation::Landscape;
            }
            if (rotation == 90 || rotation == 270) {
                displayHeight = width;
                displayWidth = height;
                displayedOrientation = Orientation::Portrait;
            }
        }
        if (width < height) {
            if (rotation == 0 || rotation == 180) {
                displayHeight = height;
                displayWidth = width;
                displayedOrientation = Orientation::Portrait;
            }
            if (rotation == 90 || rotation == 270) {
                displayHeight = width;
                displayWidth = height;
                displayedOrientation = Orientation::Landscape;
            }
        }
        if (width == height) {
            displayWidth = width;
            displayHeight = height;
            displayedOrientation = Orientation::Square;
        }
    }

    void setMetaData(KFileMetaData::PropertyMultiMap metaData)
    {
        width = metaData.value(KFileMetaData::Property::Width).toInt();
        height = metaData.value(KFileMetaData::Property::Height).toInt();
        aspectRatio = metaData.value(KFileMetaData::Property::AspectRatio).toDouble();
        frameRate = metaData.value(KFileMetaData::Property::FrameRate).toDouble();
        videoCodec = metaData.value(KFileMetaData::Property::VideoCodec).toString();
        calculateDisplayVariables();
    }
};

#endif // PLAYLISTMETADATA_H
