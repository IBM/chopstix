CREATE TABLE IF NOT EXISTS func (
    name       TEXT    NOT NULL,
    addr_begin INTEGER NOT NULL,
    addr_end   INTEGER NOT NULL,

    module_id  INTEGER NOT NULL,
    FOREIGN KEY(module_id) REFERENCES module(rowid)
);
CREATE INDEX IF NOT EXISTS func_module_id_index ON func(module_id); 

CREATE TABLE IF NOT EXISTS func_annot (
    func_id INTEGER NOT NULL,
    count   INTEGER,
    score   REAL,

    FOREIGN KEY(func_id) REFERENCES func(rowid)
);
CREATE INDEX IF NOT EXISTS func_annot_func_id_index ON func_annot(func_id); 
