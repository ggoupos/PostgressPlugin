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

    // Create the continuous aggregate view for total sales
    const char *createViewQuery = "CREATE MATERIALIZED VIEW total_sales AS SELECT SUM(sales) AS total FROM sales_data;";
    PGresult *createViewResult = PQexec(conn, createViewQuery);
    if (PQresultStatus(createViewResult) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Creating continuous aggregate view failed: %s", PQerrorMessage(conn));
        PQclear(createViewResult);
        PQfinish(conn);
        exit(1);
    }
    PQclear(createViewResult);


    // Retrieve the total sales from the continuous aggregate view
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

    // Clear the result and close the database connection
    PQclear(selectResult);
    PQfinish(conn);

    return 0;
}

