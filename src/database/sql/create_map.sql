-- DROP TABLE IF EXISTS map
CREATE TABLE IF NOT EXISTS map (
    pid        BIGINT NOT NULL,
    addr_begin BIGINT NOT NULL,
    addr_end   BIGINT NOT NULL,
    perm       TEXT    NOT NULL,
    path       TEXT    NOT NULL
);

-- DROP INDEX IF EXISTS map_pid_index
CREATE INDEX IF NOT EXISTS map_pid_index ON map(pid); 
