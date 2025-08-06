CREATE TABLE recent_files (
    recent_file_id INTEGER NOT NULL UNIQUE,
    url            TEXT NOT NULL UNIQUE,
    filename       TEXT NOT NULL,
    opened_from    TEXT NOT NULL,
    timestamp      INTEGER NOT NULL,
    PRIMARY KEY(recent_file_id)
);
