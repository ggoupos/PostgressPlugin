* In order to run everything smoothly, PostgreSQL should be installed in the system.

## [BUILDING]
The project can be built using CMake. Open a terminal, navigate to the project's root directory, and execute the following commands:
```
mkdir build
cd build
cmake ..
make
```

## [RUNNING]
# Follow the steps in order

1. Running the database setup: To init a database called 'test_db', execute the following command:
`./database_setup`

2. Running the data population: To populate the database with sample data, execute the following command:
`./data_population`
This will create the employees table and populate it with random employee names and sales data.

3. Running the continuous aggregates: To compute the total sales of all employees using continuous aggregates, execute the following command:
`./continuous_aggregates`
This will calculate and store the average sales value in the sales_avg table.

4. Running the test refresh: To test the refresh functionality of the continuous aggregates, execute the following command:
`./test_refresh`
This will add additional employees and their sales data to the employees table and refresh the continuous aggregates to reflect the updated values.
