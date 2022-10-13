-- DROP TABLE IF EXISTS sample
CREATE TABLE IF NOT EXISTS sample (
    ip   BIGINT NOT NULL,
    pid  BIGINT NOT NULL,
    tid  BIGINT NOT NULL,
    time BIGINT NOT NULL
);

-- DROP INDEX IF EXISTS sample_pid_index
CREATE INDEX IF NOT EXISTS sample_pid_index ON sample(pid); 
