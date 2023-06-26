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

    // Add more employees with sales to the sales_data table
    const char *insertQuery = "INSERT INTO sales_data (employee_name, sales) VALUES ('Employee 1000', 100), ('Employee 2000', 150);";
    PGresult *insertResult = PQexec(conn, insertQuery);
    if (PQresultStatus(insertResult) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Inserting data failed: %s", PQerrorMessage(conn));
        PQclear(insertResult);
        PQfinish(conn);
        exit(1);
    }
    PQclear(insertResult);

    // Refresh the continuous aggregate view to update the results
    const char *refreshQuery = "REFRESH MATERIALIZED VIEW total_sales;";
    PGresult *refreshResult = PQexec(conn, refreshQuery);
    if (PQresultStatus(refreshResult) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Refreshing materialized view failed: %s", PQerrorMessage(conn));
        PQclear(refreshResult);
        PQfinish(conn);
        exit(1);
    }
    PQclear(refreshResult);

    // Retrieve and print the total sales from the materialized view
    const char *selectQuery = "SELECT total FROM total_sales;";
    PGresult *selectResult = PQexec(conn, selectQuery);
   if (PQresultStatus(selectResult) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Retrieving total sales failed: %s", PQerrorMessage(conn));
        PQclear(selectResult);
        PQfinish(conn);
        exit(1);
    }

    // Check if the result has at least one row
    if (PQntuples(selectResult) > 0) {
        // Get the total sales value from the first row and first column
        int totalSales = atoi(PQgetvalue(selectResult, 0, 0));

        printf("Total sales: %d\n", totalSales);
    } else {
        printf("No data found.\n");
    }

    PQfinish(conn);
    return 0;
}

