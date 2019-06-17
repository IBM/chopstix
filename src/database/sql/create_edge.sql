CREATE TABLE IF NOT EXISTS edge (
    from_id INTEGER NOT NULL,
    to_id   INTEGER NOT NULL,

    FOREIGN KEY(from_id) REFERENCES block(rowid),
    FOREIGN KEY(to_id)   REFERENCES block(rowid),

    UNIQUE (from_id, to_id)
);
CREATE INDEX IF NOT EXISTS edge_from_id_index ON edge(from_id); 
CREATE INDEX IF NOT EXISTS edge_to_id_index ON edge(to_id); 
