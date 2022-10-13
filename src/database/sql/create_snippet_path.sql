-- DROP TABLE IF EXISTS snippet_path
CREATE TABLE IF NOT EXISTS snippet_path (
    id BIGINT NOT NULL,
    block_id BIGINT NOT NULL,
    pos BIGINT NOT NULL,
    score REAL NOT NULL,
    FOREIGN KEY(id) REFERENCES SNIPPET(rowid),
    FOREIGN KEY(block_id) REFERENCES BLOCK(rowid)
);

-- DROP INDEX IF EXISTS snippet_path_id_index
CREATE INDEX IF NOT EXISTS snippet_path_id_index ON snippet_path(id);

-- DROP INDEX IF EXISTS snippet_block_id_index
CREATE INDEX IF NOT EXISTS snippet_block_id_index ON snippet_path(block_id);
