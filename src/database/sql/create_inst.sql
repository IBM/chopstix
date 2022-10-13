-- DROP TABLE IF EXISTS inst;
CREATE TABLE IF NOT EXISTS inst (
    addr  BIGINT NOT NULL,
    rawb  TEXT    NOT NULL,
    text  TEXT    NOT NULL,

    block_id BIGINT NOT NULL,
    func_id BIGINT NOT NULL,
    module_id BIGINT NOT NULL,

    FOREIGN KEY(block_id) REFERENCES block(rowid),
    FOREIGN KEY(func_id) REFERENCES func(rowid),
    FOREIGN KEY(module_id) REFERENCES module(rowid)
);

-- DROP INDEX IF EXISTS inst_addr_index;
CREATE INDEX IF NOT EXISTS inst_addr_index ON inst(addr); 
-- DROP INDEX IF EXISTS inst_block_id_index;
CREATE INDEX IF NOT EXISTS inst_block_id_index ON inst(block_id);
-- DROP INDEX IF EXISTS inst_func_id_index;
CREATE INDEX IF NOT EXISTS inst_func_id_index ON inst(func_id);
-- DROP INDEX IF EXISTS inst_module_id_index;
CREATE INDEX IF NOT EXISTS inst_module_id_index ON inst(module_id);

-- DROP TABLE IF EXISTS inst_annot;
CREATE TABLE IF NOT EXISTS inst_annot (
    inst_id BIGINT NOT NULL,
    module_id BIGINT NOT NULL,
    module_name TEXT    NOT NULL,
    count   BIGINT,
    score   REAL,

    FOREIGN KEY(inst_id) REFERENCES inst(rowid)
);

-- DROP INDEX IF EXISTS inst_annot_inst_id_index;
CREATE INDEX IF NOT EXISTS inst_annot_inst_id_index ON inst_annot(inst_id); 
