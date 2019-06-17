CREATE TABLE IF NOT EXISTS block (
    addr_begin INTEGER NOT NULL,
    addr_end   INTEGER NOT NULL,

    func_id    INTEGER NOT NULL,
    module_id    INTEGER NOT NULL,

    FOREIGN KEY(func_id) REFERENCES func(rowid),
    FOREIGN KEY(module_id) REFERENCES module(rowid)
);
CREATE INDEX IF NOT EXISTS block_func_id_index ON block(func_id);
CREATE INDEX IF NOT EXISTS block_module_id_index ON block(module_id);

CREATE TABLE IF NOT EXISTS block_annot (
    block_id INTEGER NOT NULL,
    count    INTEGER,
    score    REAL,

    FOREIGN KEY(block_id) REFERENCES block(rowid)
);
CREATE INDEX IF NOT EXISTS block_annot_block_id_index ON block_annot(block_id);
