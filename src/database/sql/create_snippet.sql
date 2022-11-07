-- DROP TABLE IF EXISTS snippet
CREATE TABLE IF NOT EXISTS snippet (
    module_id BIGINT NOT NULL,
    func_id BIGINT NOT NULL,
    score REAL NOT NULL,
    score_bb REAL NOT NULL,
    score_in REAL NOT NULL,
    FOREIGN KEY(func_id) REFERENCES FUNC(rowid)
);

-- DROP INDEX IF EXISTS  snippet_module_id_index
CREATE INDEX IF NOT EXISTS snippet_module_id_index ON snippet(module_id);

-- DROP INDEX IF EXISTS snippet_func_id_index
CREATE INDEX IF NOT EXISTS snippet_func_id_index ON snippet(func_id);
