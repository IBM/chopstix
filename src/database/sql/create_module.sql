CREATE TABLE IF NOT EXISTS module (
    name       TEXT    NOT NULL,
    arch       TEXT    NOT NULL,
    addr_begin INTEGER NOT NULL,
    addr_end   INTEGER NOT NULL,

    UNIQUE (name, arch)
);
CREATE INDEX IF NOT EXISTS module_name_index ON module(name); 

CREATE TABLE IF NOT EXISTS module_annot (
    module_id INTEGER NOT NULL,
    count     INTEGER,
    score     REAL,

    FOREIGN KEY(module_id) REFERENCES module(rowid)
);
CREATE INDEX IF NOT EXISTS module_annot_module_id_index ON module_annot(module_id);
