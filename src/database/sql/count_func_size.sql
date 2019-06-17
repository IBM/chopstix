SELECT COUNT(*) as num_instr, func_id
FROM inst
WHERE func_id = ?
GROUP BY func_id
