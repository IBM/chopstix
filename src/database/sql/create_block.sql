-- DROP TABLE IF EXISTS block;
CREATE TABLE IF NOT EXISTS block (
    addr_begin BIGINT NOT NULL,
    addr_end   BIGINT NOT NULL,

    func_id    BIGINT NOT NULL,
    module_id    BIGINT NOT NULL,

    FOREIGN KEY(func_id) REFERENCES func(rowid),
    FOREIGN KEY(module_id) REFERENCES module(rowid)
);

-- DROP INDEX IF EXISTS block_func_id_index;
CREATE INDEX IF NOT EXISTS block_func_id_index ON block(func_id);

-- DROP INDEX IF EXISTS block_module_id_index;
CREATE INDEX IF NOT EXISTS block_module_id_index ON block(module_id);

-- DROP TABLE IF EXISTS block_annot;
CREATE TABLE IF NOT EXISTS block_annot (
    block_id BIGINT NOT NULL,
    count    BIGINT,
    score    REAL,

    FOREIGN KEY(block_id) REFERENCES block(rowid)
);

-- DROP INDEX IF EXISTS block_annot_block_id_index;
CREATE INDEX IF NOT EXISTS block_annot_block_id_index ON block_annot(block_id);
