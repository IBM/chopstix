SELECT block.rowid as the_rowid, block.addr_begin, block.addr_end,
       CASE WHEN block_annot.count IS NULL THEN 0 ELSE block_annot.count END as the_count,
       CASE WHEN block_annot.score IS NULL THEN 0 ELSE block_annot.score END as the_score,
       block.func_id
FROM block LEFT JOIN block_annot
ON block.rowid = block_annot.block_id
