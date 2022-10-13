SELECT inst.rowid AS inst_id,
       SUM(_sample_grouped.count) AS count,
           0 AS score
 FROM inst INNER JOIN _sample_grouped
 ON _sample_grouped.addr = inst.addr
 WHERE _sample_grouped.module_id = ?
 AND _sample_grouped.raw_addr = inst.addr + _sample_grouped.mo
 GROUP BY inst.rowid
