UPDATE inst_annot
SET score = 1.0 * count / (SELECT SUM(count) FROM inst_annot);
