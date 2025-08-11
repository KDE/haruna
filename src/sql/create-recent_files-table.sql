-- SPDX-FileCopyrightText: 2025 George Florea Bănuș <georgefb899@gmail.com>
-- SPDX-License-Identifier: CC-BY-4.0

CREATE TABLE recent_files (
    recent_file_id INTEGER NOT NULL UNIQUE,
    url            TEXT NOT NULL UNIQUE,
    filename       TEXT NOT NULL,
    opened_from    TEXT NOT NULL,
    timestamp      INTEGER NOT NULL,
    PRIMARY KEY(recent_file_id)
);
