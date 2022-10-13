-- DROP TABLE IF EXISTS module;
CREATE TABLE IF NOT EXISTS module (
    name       TEXT    NOT NULL,
    arch       TEXT    NOT NULL,
    addr_begin BIGINT NOT NULL,
    addr_end   BIGINT NOT NULL,

    UNIQUE (name, arch)
);

-- DROP INDEX IF EXISTS module_name_index;
CREATE INDEX IF NOT EXISTS module_name_index ON module(name);

-- DROP TABLE IF EXISTS module_annot;
CREATE TABLE IF NOT EXISTS module_annot (
    module_id BIGINT NOT NULL,
    count     BIGINT,
    score     REAL,

    FOREIGN KEY(module_id) REFERENCES module(rowid)
);

-- DROP INDEX IF EXISTS module_annot_module_id_index;
CREATE INDEX IF NOT EXISTS module_annot_module_id_index ON module_annot(module_id);
