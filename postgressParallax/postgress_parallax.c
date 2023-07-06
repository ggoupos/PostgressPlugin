#include <postgres.h>
#include "../../parallax/lib/include/parallax/parallax.h"
#include "../../parallax/lib/include/parallax/structures.h"
#include <stdlib.h>
#include <fmgr.h>
#include <catalog/pg_type.h>
#include <executor/spi.h>
#include <string.h>
#include <utils/builtins.h>
#include <utils/elog.h>

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(postgress_parallax);

Datum postgress_parallax(PG_FUNCTION_ARGS) {

  text* table_name = PG_GETARG_TEXT_P(0);
  text* key_column = PG_GETARG_TEXT_P(1);
  text* value_column = PG_GETARG_TEXT_P(2);

  char* table_name_str = text_to_cstring(table_name);
  char* key_column_str = text_to_cstring(key_column);
  char* value_column_str = text_to_cstring(value_column);

  elog(NOTICE, "Table name: %s", table_name_str); // Print debug information
  elog(NOTICE, "Key column name: %s", key_column_str); // Print debug information
  elog(NOTICE, "Value column name: %s", value_column_str); // Print debug information
  
  const char* error_msg = NULL;

   struct par_db_options opt = { .volume_name = "test.dat",
                      .db_name = "test_postgress",
                      .create_flag = PAR_CREATE_DB,
                      .options = par_get_default_options()};

  par_format("test.dat",128);

  par_handle handle = par_open(&opt,&error_msg);

 // PG_RETURN_NULL();

  char* query = psprintf("SELECT %s, %s FROM %s", key_column_str, value_column_str, table_name_str);

  SPI_connect();
  SPI_execute(query, true, 0);

    if (SPI_processed == 0) {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("No rows returned for table '%s'", table_name_str)));
    }

    TupleDesc tuple_desc = SPI_tuptable->tupdesc;
    int key_index = SPI_fnumber(tuple_desc, key_column_str);
    int value_index = SPI_fnumber(tuple_desc, value_column_str);

    if (key_index == SPI_ERROR_NOATTRIBUTE) {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("Column '%s' does not exist in table '%s'", key_column_str, table_name_str)));
    }

    if (value_index == SPI_ERROR_NOATTRIBUTE) {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("Column '%s' does not exist in table '%s'", value_column_str, table_name_str)));
    }

    Datum* keys = palloc(SPI_processed * sizeof(Datum));
    Datum* values = palloc(SPI_processed * sizeof(Datum));
    bool* nulls = palloc(SPI_processed * sizeof(bool));

    struct par_key_value kv;

    for (int i = 0; i < SPI_processed; i++) {
        HeapTuple tuple = SPI_tuptable->vals[i];

        keys[i] = SPI_getbinval(tuple, tuple_desc, key_index, &nulls[i]);
        char* key_str = text_to_cstring(DatumGetTextP(keys[i]));
        kv.k.data = strdup(key_str);
        kv.k.size = strlen(key_str) + 1;

        values[i] = SPI_getbinval(tuple, tuple_desc, value_index, &nulls[i]);
        char* value_str = text_to_cstring(DatumGetTextP(values[i]));
        kv.v.val_size = strlen(value_str) + 1;
        kv.v.val_buffer = strdup(value_str);
        kv.v.val_buffer_size = 0;

        par_put(handle, &kv, &error_msg) ;

        elog(NOTICE, "Key %d = %s  Value %d = %s", i, key_str, i, value_str); // Print debug information
        

    }

    par_close(handle);

    SPI_finish();
    pfree(values);
    pfree(nulls);
    pfree(query);
    pfree(table_name_str);
    pfree(key_column_str);
    pfree(value_column_str);
    

    PG_RETURN_NULL();
  }

