-- DROP TABLE IF EXISTS session
CREATE TABLE IF NOT EXISTS session (
    pid BIGINT NOT NULL,
    cmd TEXT    NOT NULL
);

-- DROP INDEX IF EXISTS session_pid_index
CREATE INDEX IF NOT EXISTS session_pid_index ON session(pid); 
