DROP TABLE IF EXISTS inst_annot;

CREATE TABLE IF NOT EXISTS inst_annot (
   inst_id BIGINT NOT NULL,
   module_id BIGINT NOT NULL,
   module_name TEXT    NOT NULL,
   count   BIGINT,
   score   REAL,

   FOREIGN KEY(inst_id) REFERENCES inst(rowid)
);

DROP INDEX IF EXISTS inst_annot_inst_id_index;
CREATE INDEX IF NOT EXISTS inst_annot_inst_id_index ON inst_annot(inst_id);

DELETE FROM inst_annot;
