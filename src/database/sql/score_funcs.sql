UPDATE func_annot
SET score = (
    SELECT SUM(block_annot.score)
    FROM block
    JOIN func
        ON block.func_id = func.rowid
    LEFT JOIN block_annot
        ON block.rowid = block_annot.block_id
    WHERE func.rowid = func_annot.func_id
);