CREATE TABLE IF NOT EXISTS session (
    pid INTEGER NOT NULL,
    cmd TEXT    NOT NULL
);
CREATE INDEX IF NOT EXISTS session_pid_index ON session(pid); 
