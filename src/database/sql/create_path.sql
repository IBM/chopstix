-- DROP TABLE IF EXISTS path
CREATE TABLE IF NOT EXISTS path (
    hash  BIGINT NOT NULL,
    score REAL
);

-- DROP INDEX IF EXISTS path_hash_index
CREATE INDEX IF NOT EXISTS path_hash_index ON path(hash);

-- DROP TABLE IF EXISTS path_node
CREATE TABLE IF NOT EXISTS path_node (
    path_id  BIGINT NOT NULL,
    block_id BIGINT NOT NULL,
    rank     BIGINT NOT NULL,
    FOREIGN KEY(path_id)  REFERENCES path(rowid),
    FOREIGN KEY(block_id) REFERENCES block(rowid)
);

-- DROP INDEX IF EXISTS path_node_path_id_index
CREATE INDEX IF NOT EXISTS path_node_path_id_index ON path_node(path_id);

-- DROP INDEX IF EXISTS path_node_block_id_index
CREATE INDEX IF NOT EXISTS path_node_block_id_index ON path_node(block_id);

