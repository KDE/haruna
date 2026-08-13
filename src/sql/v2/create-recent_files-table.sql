-- SPDX-FileCopyrightText: 2025 George Florea Bănuș <georgefb899@gmail.com>
-- SPDX-License-Identifier: CC-BY-4.0

CREATE TABLE recent_files (
    recent_file_id INTEGER PRIMARY KEY,
    url_id         INTEGER NOT NULL UNIQUE,
    filename       TEXT NOT NULL,
    opened_from    TEXT NOT NULL,
    timestamp      INTEGER NOT NULL,

    CONSTRAINT url_id_fk
        FOREIGN KEY(url_id)
        REFERENCES urls(url_id)
        ON DELETE CASCADE
        ON UPDATE CASCADE
);
