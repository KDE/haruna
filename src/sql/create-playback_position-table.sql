-- SPDX-FileCopyrightText: 2025 George Florea Bănuș <georgefb899@gmail.com>
-- SPDX-License-Identifier: CC-BY-4.0

CREATE TABLE playback_position (
    md5_hash TEXT NOT NULL UNIQUE,
    path     TEXT NOT NULL,
    position REAL NOT NULL,
    PRIMARY KEY(md5_hash)
);
