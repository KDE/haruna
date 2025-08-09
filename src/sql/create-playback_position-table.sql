CREATE TABLE playback_position (
    md5_hash TEXT NOT NULL UNIQUE,
    path     TEXT NOT NULL,
    position REAL NOT NULL,
    PRIMARY KEY(md5_hash)
);
