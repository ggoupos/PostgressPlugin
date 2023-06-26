# Median Calculation extension 

## Description

A simple PostgresExtension that computes the median of a table.

## Installation

```
git clone https://github.com/ggoupos/PostgressPlugin.git
cd median_calculation
make
sudo make install
```

## Usage

In order to use the median_calculation extension in Postgres, follow these steps:

1. In order to test the extension, you'll need to access your PostgreSQL database. You can use the psql command with the appropriate user (in this case, "postgres") to enter the PostgreSQL console.
```
psql -U postgres

```
2. Load the extension
```
CREATE EXTENSION median_calculation;

```
3. Create a test table
```
CREATE TABLE test_table (id SERIAL, value integer);
INSERT INTO test_table (value) VALUES (10), (5), (7), (9), (12);

```
4. Use the extension
```
SELECT median_calculation('test_table', 'value');

```

    
