CREATE TABLE IF NOT EXISTS inst (
    addr  INTEGER NOT NULL,
    rawb  TEXT    NOT NULL,
    text  TEXT    NOT NULL,

    block_id INTEGER NOT NULL,
    func_id INTEGER NOT NULL,
    module_id INTEGER NOT NULL,

    FOREIGN KEY(block_id) REFERENCES block(rowid),
    FOREIGN KEY(func_id) REFERENCES func(rowid),
    FOREIGN KEY(module_id) REFERENCES module(rowid)
);
CREATE INDEX IF NOT EXISTS inst_addr_index ON inst(addr); 
CREATE INDEX IF NOT EXISTS inst_block_id_index ON inst(block_id);
CREATE INDEX IF NOT EXISTS inst_func_id_index ON inst(func_id);
CREATE INDEX IF NOT EXISTS inst_module_id_index ON inst(module_id);

CREATE TABLE IF NOT EXISTS inst_annot (
    inst_id INTEGER NOT NULL,
    count   INTEGER,
    score   REAL,

    FOREIGN KEY(inst_id) REFERENCES inst(rowid)
);
CREATE INDEX IF NOT EXISTS inst_annot_inst_id_index ON inst_annot(inst_id); 
