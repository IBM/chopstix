SELECT inst.rowid as the_rowid, inst.addr, inst.rawb, inst.text,
       CASE WHEN inst_annot.count IS NULL THEN 0 ELSE inst_annot.count END as the_count,
       CASE WHEN inst_annot.score IS NULL THEN 0 ELSE inst_annot.score END as the_score,
       inst.block_id
FROM inst LEFT JOIN inst_annot
ON inst.rowid = inst_annot.inst_id
