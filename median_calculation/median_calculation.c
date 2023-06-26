#include <postgres.h>
#include <stdlib.h>
#include <fmgr.h>
#include <catalog/pg_type.h>
#include <executor/spi.h>
#include <utils/builtins.h>
#include <utils/elog.h>

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(median_calculation);

static int cmp_numeric(const void* a, const void* b); 

Datum median_calculation(PG_FUNCTION_ARGS) {
    text* table_name = PG_GETARG_TEXT_P(0);
    text* column_name = PG_GETARG_TEXT_P(1);

    char* table_name_str = text_to_cstring(table_name);
    char* column_name_str = text_to_cstring(column_name);

  elog(NOTICE, "Table name: %s", table_name_str); // Print debug information
  elog(NOTICE, "Column name: %s", column_name_str); // Print debug information


    char* query = psprintf("SELECT %s FROM %s", column_name_str, table_name_str);

    SPI_connect();
    SPI_execute(query, true, 0);

    if (SPI_processed == 0) {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("No rows returned for table '%s'", table_name_str)));
    }

    TupleDesc tuple_desc = SPI_tuptable->tupdesc;
    int column_index = SPI_fnumber(tuple_desc, column_name_str);
    if (column_index == SPI_ERROR_NOATTRIBUTE) {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("Column '%s' does not exist in table '%s'", column_name_str, table_name_str)));
    }

    Datum* values = palloc(SPI_processed * sizeof(Datum));
    bool* nulls = palloc(SPI_processed * sizeof(bool));

    for (int i = 0; i < SPI_processed; i++) {
        HeapTuple tuple = SPI_tuptable->vals[i];
        values[i] = SPI_getbinval(tuple, tuple_desc, column_index, &nulls[i]);
        elog(NOTICE, "Value %d = %ld ",i, DatumGetInt64(values[i])); // Print debug information
    }

    
    elog(NOTICE, "START SORTING"); // Print debug information
    qsort(values, SPI_processed, sizeof(Datum), cmp_numeric);
    elog(NOTICE, "FINISH SORTING"); // Print debug information


    Datum median;
    if (SPI_processed % 2 == 0) {
        Datum value1 = values[(SPI_processed / 2) - 1];
        Datum value2 = values[SPI_processed / 2];
        float8 median_float = ((float8)DatumGetInt64(value1) + (float8)DatumGetInt64(value2)) / 2.0;
        SPI_finish();
        pfree(values);
        pfree(nulls);
        pfree(query);
        pfree(table_name_str);
        pfree(column_name_str);
        PG_RETURN_FLOAT8(median_float);
    }
    else {
        median = values[SPI_processed / 2];

    }
    PG_RETURN_FLOAT8(median);
}

static int cmp_numeric(const void* a, const void* b) {
  Datum da = *(const Datum *)a;
  Datum db = *(const Datum *)b;

  int64 va = DatumGetInt64(da);
  int64 vb = DatumGetInt64(db);

   if (va < vb) {
        return -1;
    } else if (va > vb) {
        return 1;
    } else {
        return 0;
    }

}
