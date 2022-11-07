DELETE FROM block_annot;

INSERT INTO block_annot
SELECT inst.block_id,
       SUM(inst_annot.count) as count,
       0 as score
FROM inst_annot INNER JOIN inst
ON inst.rowid = inst_annot.inst_id
AND inst.module_id = inst_annot.module_id
GROUP BY inst.block_id;
