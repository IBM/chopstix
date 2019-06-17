DELETE FROM module_annot;

INSERT INTO module_annot
SELECT func.module_id,
       SUM(func_annot.count) as count,
       0 as score
FROM func INNER JOIN func_annot
ON func.rowid = func_annot.func_id
GROUP BY func.module_id;