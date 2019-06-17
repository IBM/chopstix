SELECT module.rowid as the_rowid, module.name, module.arch,
       module.addr_begin, module.addr_end,
       CASE WHEN module_annot.count IS NULL THEN 0 ELSE module_annot.count END as the_count,
       CASE WHEN module_annot.score IS NULL THEN 0 ELSE module_annot.score END as the_score
FROM module LEFT JOIN module_annot
ON module.rowid = module_annot.module_id
