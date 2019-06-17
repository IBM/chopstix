CREATE TABLE IF NOT EXISTS map (
    pid        INTEGER NOT NULL,
    addr_begin INTEGER NOT NULL,
    addr_end   INTEGER NOT NULL,
    perm       TEXT    NOT NULL,
    path       TEXT    NOT NULL
);
CREATE INDEX IF NOT EXISTS map_pid_index ON map(pid); 
