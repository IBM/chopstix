-- DROP TABLE IF EXISTS edge;
CREATE TABLE IF NOT EXISTS edge (
    from_id BIGINT NOT NULL,
    to_id   BIGINT NOT NULL,

    FOREIGN KEY(from_id) REFERENCES block(rowid),
    FOREIGN KEY(to_id)   REFERENCES block(rowid),

    UNIQUE (from_id, to_id)
);

-- DROP INDEX IF EXISTS edge_from_id_index;
CREATE INDEX IF NOT EXISTS edge_from_id_index ON edge(from_id); 

-- DROP INDEX IF EXISTS edge_to_id_index;
CREATE INDEX IF NOT EXISTS edge_to_id_index ON edge(to_id); 
