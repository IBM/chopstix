UPDATE module_annot
SET score = (
    SELECT SUM(func_annot.score)
    FROM func
    JOIN module
        ON func.module_id = module.rowid
    LEFT JOIN func_annot
        ON func.rowid = func_annot.func_id
    WHERE module.rowid = module_annot.module_id
);
