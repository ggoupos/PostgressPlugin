#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>

int main() {
    // Establish a connection to the test_db database
    PGconn *conn = PQconnectdb("postgresql://postgres@localhost/test_db");
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connection to database failed: %s", PQerrorMessage(conn));
        PQfinish(conn);
        exit(1);
    }

    // Create the sales_data table
    const char *createTableQuery = "CREATE TABLE sales_data ("
                                   "id SERIAL PRIMARY KEY,"
                                   "employee_name VARCHAR(100),"
                                   "sales INTEGER);";
    PGresult *createTableResult = PQexec(conn, createTableQuery);
    if (PQresultStatus(createTableResult) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Table creation failed: %s", PQerrorMessage(conn));
        PQclear(createTableResult);
        PQfinish(conn);
        exit(1);
    }
    PQclear(createTableResult);

    // Populate the sales_data table with random data
    const int numRecords = 100;
    for (int i = 1; i <= numRecords; ++i) {
        char employeeName[100];
        snprintf(employeeName, sizeof(employeeName), "Employee %d", i);

        int sales = rand() % 1000 + 1;

        char insertQuery[200];
        snprintf(insertQuery, sizeof(insertQuery), "INSERT INTO sales_data (employee_name, sales) "
                                                   "VALUES ('%s', %d);", employeeName, sales);

        PGresult *insertResult = PQexec(conn, insertQuery);
        if (PQresultStatus(insertResult) != PGRES_COMMAND_OK) {
            fprintf(stderr, "Data insertion failed: %s", PQerrorMessage(conn));
            PQclear(insertResult);
            PQfinish(conn);
            exit(1);
        }
        PQclear(insertResult);
    }

    // Close the database connection
    PQfinish(conn);

    printf("Data population completed successfully!\n");

    return 0;
}
