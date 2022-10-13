-- DROP TABLE IF EXISTS func;
CREATE TABLE IF NOT EXISTS func (
    name       TEXT    NOT NULL,
    addr_begin BIGINT NOT NULL,
    addr_end   BIGINT NOT NULL,

    module_id  BIGINT NOT NULL,
    FOREIGN KEY(module_id) REFERENCES module(rowid)
);

-- DROP INDEX IF EXISTS func_module_id_index;
CREATE INDEX IF NOT EXISTS func_module_id_index ON func(module_id); 

-- DROP TABLE IF EXISTS func_annot;
CREATE TABLE IF NOT EXISTS func_annot (
    func_id BIGINT NOT NULL,
    count   BIGINT,
    score   REAL,

    FOREIGN KEY(func_id) REFERENCES func(rowid)
);

-- DROP INDEX IF EXISTS func_annot_func_id_index;
CREATE INDEX IF NOT EXISTS func_annot_func_id_index ON func_annot(func_id); 
