INSERT INTO inst_annot
SELECT inst.rowid AS inst_id,
       SUM(smp.count) AS count,
       0 AS score
FROM inst INNER JOIN _sample_grouped smp
ON smp.addr = inst.addr 
WHERE smp.module_id = ?
GROUP BY inst.rowid
