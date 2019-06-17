CREATE TABLE IF NOT EXISTS snippet (
    module_id INTEGER NOT NULL,
    func_id INTEGER NOT NULL,
    score REAL NOT NULL,
    score_bb REAL NOT NULL,
    score_in REAL NOT NULL,
    FOREIGN KEY(func_id) REFERENCES FUNC(rowid)
);
CREATE INDEX IF NOT EXISTS snippet_module_id_index ON snippet(module_id); 
CREATE INDEX IF NOT EXISTS snippet_func_id_index ON snippet(func_id); 
