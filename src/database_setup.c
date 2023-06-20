#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>

int main() {
    // Establish a connection to the PostgreSQL database
    PGconn *conn = PQconnectdb("postgresql://postgres@localhost/postgres");
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connection to database failed: %s", PQerrorMessage(conn));
        PQfinish(conn);
        exit(1);
    }

    // Create the test_db database
    const char *createDbQuery = "CREATE DATABASE test_db;";
    PGresult *createDbResult = PQexec(conn, createDbQuery);
    if (PQresultStatus(createDbResult) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Database creation failed: %s", PQerrorMessage(conn));
        PQclear(createDbResult);
        PQfinish(conn);
        exit(1);
    }
    PQclear(createDbResult);

    // Disconnect from the default "postgres" database
    PQfinish(conn);

    printf("Database setup completed successfully!\n");

    return 0;
}
