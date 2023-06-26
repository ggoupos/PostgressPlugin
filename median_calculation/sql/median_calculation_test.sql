CREATE EXTENSION median_calculation;
CREATE TABLE test_table (id SERIAL, value integer);
INSERT INTO test_table (value) VALUES (10), (5), (7), (9), (12);
SELECT median_calculation('test_table', 'value');
INSERT INTO test_table (value) VALUES (1);
SELECT median_calculation('test_table', 'value');
INSERT INTO test_table (value) VALUES (100);
SELECT median_calculation('test_table', 'value');
INSERT INTO test_table (value) VALUES (74);
SELECT median_calculation('test_table', 'value');
INSERT INTO test_table (value) VALUES (6);
SELECT median_calculation('test_table', 'value');

