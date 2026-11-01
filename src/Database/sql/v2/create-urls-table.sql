-- SPDX-FileCopyrightText: 2026 George Florea Bănuș <georgefb899@gmail.com>
-- SPDX-License-Identifier: CC-BY-4.0

CREATE TABLE urls (
    url_id INTEGER PRIMARY KEY,
    url    TEXT NOT NULL UNIQUE
);
