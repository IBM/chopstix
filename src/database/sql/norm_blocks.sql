UPDATE block_annot
SET score = score / (
    SELECT COUNT(*)
    FROM inst
    JOIN block
        ON inst.block_id = block.rowid
    WHERE block.rowid = block_annot.block_id
);

CREATE TEMP TABLE block_tot_score AS
SELECT SUM(score) AS tot_score FROM block_annot;

UPDATE block_annot
SET score = score / (
    SELECT tot_score from block_tot_score
);
