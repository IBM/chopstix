DELETE FROM func_annot;

INSERT INTO func_annot
SELECT block.func_id,
       SUM(block_annot.count) as count,
       0 as score
FROM block INNER JOIN block_annot
ON block.rowid = block_annot.block_id
GROUP BY block.func_id;