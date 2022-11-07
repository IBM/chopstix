SELECT func.rowid as the_rowid, func.name,
       func.addr_begin, func.addr_end,
       CASE WHEN func_annot.count IS NULL THEN 0 ELSE func_annot.count END as the_count,
       CASE WHEN func_annot.score IS NULL THEN 0 ELSE func_annot.score END as the_score,
       func.module_id
FROM func
LEFT JOIN func_annot ON func.rowid = func_annot.func_id
WHERE the_score >= ? AND func.rowid IN (
    SELECT func_id
    FROM inst
    GROUP BY func_id
    HAVING COUNT(*) >=?
)
ORDER BY the_score DESC
LIMIT ?;
