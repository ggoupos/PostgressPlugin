DROP EXTENSION postgress_parallax;
DROP TABLE test_table;
CREATE EXTENSION postgress_parallax;
CREATE TABLE  test_table (
    key_column VARCHAR(255),
    value_column VARCHAR(255)
);
INSERT INTO test_table (key_column, value_column)
VALUES ('key2', 'value2'),
       ('key3', 'value3'),
       ('key4', 'value4');
select postgress_parallax('test_table','key_column', 'value_column');
