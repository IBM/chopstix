DELETE FROM block_annot;

INSERT INTO block_annot
SELECT inst.block_id,
       SUM(inst_annot.count) as count,
       0 as score
FROM inst INNER JOIN inst_annot
ON inst.rowid = inst_annot.inst_id
GROUP BY inst.block_id;