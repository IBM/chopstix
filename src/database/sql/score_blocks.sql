UPDATE block_annot
SET score = 1.0 * count / (SELECT SUM(count) FROM block_annot);
