CREATE TABLE IF NOT EXISTS snippet_path (
    id INTEGER NOT NULL,
    block_id INTEGER NOT NULL,
    pos INTEGER NOT NULL,
    score REAL NOT NULL,
    FOREIGN KEY(id) REFERENCES SNIPPET(rowid),
    FOREIGN KEY(block_id) REFERENCES BLOCK(rowid)
);
CREATE INDEX IF NOT EXISTS snippet_path_id_index ON snippet_path(id); 
CREATE INDEX IF NOT EXISTS snippet_block_id_index ON snippet_path(block_id); 
