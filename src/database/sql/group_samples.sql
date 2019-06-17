-- Map modules to memory regions
DROP TABLE IF EXISTS _module_map;
CREATE TEMP TABLE _module_map AS
SELECT module.rowid      AS module_id,
       module.addr_begin AS addr_begin,
       module.addr_end   AS addr_end,
       module.name       AS name,
       map.pid           AS pid,
       map.addr_begin    AS map_begin,
       map.addr_end      AS map_end,
       map.addr_begin    AS offset
FROM map INNER JOIN module
ON map.path = module.name;
CREATE INDEX IF NOT EXISTS _module_map_module_id_index ON _module_map(module_id);
CREATE INDEX IF NOT EXISTS _module_map_pid_index ON _module_map(pid);

-- Fix mapping where necessary
UPDATE _module_map
SET offset = 0
WHERE addr_begin BETWEEN map_begin AND map_end
AND   addr_end   BETWEEN map_begin AND map_end;
-- Group and weigh samples by PC
DROP TABLE IF EXISTS _sample_grouped;
CREATE TEMP TABLE _sample_grouped AS
SELECT sample.pid         AS pid,
       module_id          AS module_id,
       sample.ip - offset AS addr,
       COUNT(*)           AS count
FROM _module_map INNER JOIN sample
ON _module_map.pid = sample.pid
AND sample.ip BETWEEN map_begin AND map_end
GROUP BY sample.pid, module_id, sample.ip;

CREATE INDEX IF NOT EXISTS _sample_grouped_pid_index ON _sample_grouped(pid);
CREATE INDEX IF NOT EXISTS _sample_grouped_module_id_index ON _sample_grouped(module_id);
CREATE INDEX IF NOT EXISTS _sample_grouped_addr_index ON _sample_grouped(addr ASC);

-- DROP TABLE IF EXISTS _sample_grouped_all_info;
-- CREATE TEMP TABLE _sample_grouped_all_info AS
-- SELECT smp.pid, smp.module_id, smp.addr

