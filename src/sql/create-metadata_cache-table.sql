-- SPDX-FileCopyrightText: 2026 George Florea Bănuș <georgefb899@gmail.com>
-- SPDX-License-Identifier: CC-BY-4.0

CREATE TABLE metadata_cache (
    metadata_id         INTEGER NOT NULL UNIQUE,
    url                 TEXT NOT NULL UNIQUE,
    title               TEXT,
    duration            INTEGER NOT NULL,
    formatted_duration  TEXT,
    width               INTEGER,
    height              INTEGER,
    aspect_ratio        REAL,
    framerate           REAL,
    video_codec         TEXT,
    orientation         INTEGER,
    track_no            INTEGER,
    disc_no             INTEGER,
    album               TEXT,
    artist              TEXT,
    album_artist        TEXT,
    audio_codec         TEXT,
    bitrate             INTEGER,
    sample_rate         INTEGER,
    genre               TEXT,
    release_year        INTEGER,
    composer            TEXT,
    lyricist            TEXT,
    PRIMARY KEY(metadata_id)
);