CREATE TABLE IF NOT EXISTS sample (
    ip   INTEGER NOT NULL,
    pid  INTEGER NOT NULL,
    tid  INTEGER NOT NULL,
    time INTEGER NOT NULL
);
CREATE INDEX IF NOT EXISTS sample_pid_index ON sample(pid); 
