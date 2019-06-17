# Database layout

ChopStiX saves all collected information into a SQLite database.
The `src/database/sql` folder contains SQL snippets and commands that can be used from C++ code.

### Example

Consider a SQL snippet file named `src/database/sql/foo_bar.sql` We need to add
`foo_bar` to `src/database/CMakeLists.txt`. Now the corresponding SQL file will
get transpiled into a header file (`<build_dir>/sql/foo_bar.h`). For
convenience, all SQL queries get bundled into a single header file
(`<build_dir>/queries.h`) that can be included from C++ code. We can then use
this SQL snippet via the `SQL_FOO_BAR` macro.

Most tables have corresponding snippets to create, insert, and select objects.

## Sampling information

The following are the tables associated with information obtained by sampling
an application, i.e. by running `chop sample ...`.

### Table: session

A session is created whenever you run `chop sample`.

| Field | Type | Description                |
| ----- | ---- | -------------------------- |
| pid   | int  | Process ID of this session |
| cmd   | text | Command being run          |

### Table: map

Saves the memory layout (/proc/<pid>/maps).

| Field      | Type | Description                       |
| ---------- | ---- | --------------------------------- |
| pid        | int  | Process ID for this map           |
| addr_begin | int  | Begin of memory region            |
| addr_end   | int  | End of memory region              |
| perm       | text | Permissions of region (e.g. rw-p) |
| path       | text | Path to object file/binary        |

### Table: sample

Saves collected samples.

| Field    | Type | Description                           |
| -------- | ---- | ------------------------------------- |
| ip       | int  | Instruction pointer, i.e. PC          |
| pid      | int  | Process ID of session                 |
| tid      | int  | Thread ID (should be the same as pid) |
| time     | int  | Sampling timestamp                    |
| events.. | int  | Each sampled event has its own column |

## Control flow graph information

ChopStiX saves the CFG in a tree-like fashion, i.e. a module (binary/library)
consists of functions, functions consist of basic blocks, and basic blocks
consist of instructions. Edges, i.e. links between blocks are currently not
saved, and need to be rebuild when necessary.

The annotation is saved in a different table, suffixed by `_annot`. This is in
order to speedup the counting process, since a large number of
instructions/basic blocks/functions/modules will never be sampled.

### Table: module

Saves module information, i.e. an object file or binary.

| Field        | Type   | Description                                    |
| ------------ | ------ | ---------------------------------------------- |
| name         | text   | Name, i.e. path of module                      |
| arch         | text   | Architecture descriptor, e.g. powerpc,x86_64   |
| addr_begin   | int    | First address in module                        |
| addr_end     | int    | Last address in module                         |

| Field     | Type | Description                      |
| --------- | ---- | -------------------------------- |
| module_id | int  | Reference to module              |
| count     | int  | Number of samples in this module |
| score     | real | Score for this module            |

### Table: func

Saves function information.

| Field        | Type   | Description                  |
| ------------ | ------ | ---------------------------- |
| name         | text   | Function name/label          |
| addr_begin   | int    | First address in function    |
| addr_end     | int    | Last address in function     |
| module_id    | int    | Reference to parent module   |

| Field   | Type | Description                        |
| ------- | ---- | ---------------------------------- |
| func_id | int  | Reference to function              |
| count   | int  | Number of samples in this function |
| score   | real | Score for this function            |

### Table: block

Saves basic block information.

| Field      | Type | Description                  |
| ---------- | ---- | ---------------------------- |
| addr_begin | int  | First address of basic block |
| addr_end   | int  | Last address of basic block  |
| func_id    | int  | Reference to parent function |

| Field    | Type | Description                           |
| -------- | ---- | ------------------------------------- |
| block_id | int  | Reference to basic block              |
| count    | int  | Number of samples in this basic block |
| score    | real | Score for this basic block            |

### Table: edge

Saves edges, i.e. control flow between basic blocks

| Field   | Type | Description        |
| ------- | ---- | ------------------ |
| from_id | int  | ID of origin block |
| to_id   | int  | ID of target block |

### Table: inst

Saves instruction information

| Field    | Type | Description                     |
| -------- | ---- | ------------------------------- |
| addr     | int  | Static address of instruction   |
| rawb     | text | Raw bytes (opcode)              |
| text     | text | Mnemonic + operands             |
| block_id | int  | Reference to parent basic block |

| Field   | Type | Description                           |
| ------- | ---- | ------------------------------------- |
| inst_id | int  | Reference to instruction              |
| count   | int  | Number of samples in this instruction |
| score   | real | Score for this instruction            |

## Path/snippet information

### Table: path

Saves created paths. Each path has a hash value to easily identify duplicates.

| Field | Type | Description                        |
| ----- | ---- | ---------------------------------- |
| hash  | int  | Calculated hash (xor of block ids) |
| score | real | Sum of individual block scores     |
| ----- | ---- | ---------------------------------- |

### Table: path_node

List of nodes (basic blocks) that belong to a path

| Field    | Type | Description                    |
| -------- | ---- | ------------------------------ |
| path_id  | int  | Reference to path              |
| block_id | int  | Reference to basic block       |
| rank     | int  | Index of the block in the path |
