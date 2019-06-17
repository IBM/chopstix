-- drop table if exists path;
-- drop table if exists path_node;

CREATE TABLE IF NOT EXISTS path (
    hash  INTEGER NOT NULL,
    score REAL
);
CREATE INDEX IF NOT EXISTS path_hash_index ON path(hash); 

CREATE TABLE IF NOT EXISTS path_node (
    path_id  INTEGER NOT NULL,
    block_id INTEGER NOT NULL,
    rank     INTEGER NOT NULL,
    FOREIGN KEY(path_id)  REFERENCES path(rowid),
    FOREIGN KEY(block_id) REFERENCES block(rowid)
);
CREATE INDEX IF NOT EXISTS path_node_path_id_index ON path_node(path_id); 
CREATE INDEX IF NOT EXISTS path_node_block_id_index ON path_node(block_id); 

