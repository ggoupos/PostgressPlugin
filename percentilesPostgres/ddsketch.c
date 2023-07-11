#include "postgres.h"
#include "fmgr.h"
#include "utils/builtins.h"
#include "utils/array.h"
#include "executor/spi.h"
#include "utils/memutils.h"
#include <float.h>

typedef struct {
    double range_start;
    double range_end;
    int count;
} Bucket;

typedef struct {
    double error_rate;
    Bucket* buckets;
    double min_value;
    double max_value;
    int num_buckets;
    int max_buckets;
    MemoryContext memory_context;
} DDSketch;


int get_total_count(DDSketch* sketch) {
    int total_count = 0;

    int i;
    for (i = 0; i < sketch->num_buckets; i++) {
        total_count += sketch->buckets[i].count;
    }

    return total_count;
}

DDSketch* create_ddsketch(double error_rate, int max_buckets) {
    DDSketch* sketch = (DDSketch*)MemoryContextAlloc(TopMemoryContext, sizeof(DDSketch));
    sketch->error_rate = error_rate;
    sketch->buckets = (Bucket*)MemoryContextAlloc(TopMemoryContext, max_buckets * sizeof(Bucket));
    sketch->min_value = DBL_MAX;
    sketch->max_value = -DBL_MAX;
    sketch->num_buckets = 0;
    sketch->max_buckets = max_buckets;
    sketch->memory_context = TopMemoryContext;
    return sketch;
}

void destroy_ddsketch(DDSketch* sketch) {
    MemoryContextDelete(sketch->memory_context);
}

void add_data_point(DDSketch* sketch, double value) {
    if (value < sketch->min_value) {
        sketch->min_value = value;
    }
    if (value > sketch->max_value) {
        sketch->max_value = value;
    }

    int i;
    for (i = 0; i < sketch->num_buckets; i++) {
        if (value >= sketch->buckets[i].range_start && value <= sketch->buckets[i].range_end) {
            sketch->buckets[i].count++;
            return;
        }
    }

    if (sketch->num_buckets == sketch->max_buckets) {
        return;
    }

    Bucket new_bucket;
    double range_width = (sketch->max_value - sketch->min_value) * sketch->error_rate;
    new_bucket.range_start = sketch->min_value - range_width;
    new_bucket.range_end = sketch->max_value + range_width;
    new_bucket.count = 1;

    sketch->buckets[sketch->num_buckets] = new_bucket;
    sketch->num_buckets++;
}

double estimate_percentile(DDSketch* sketch, double percentile) {
    int threshold_count = (int)(percentile * get_total_count(sketch));
    int cumulative_count = 0;

    int i;
    for (i = 0; i < sketch->num_buckets; i++) {
        cumulative_count += sketch->buckets[i].count;

        if (cumulative_count >= threshold_count) {
            double range_start = sketch->buckets[i].range_start;
            double range_end = sketch->buckets[i].range_end;
            int range_count = sketch->buckets[i].count;

            double percentile_value = range_start + ((range_end - range_start) * (threshold_count - cumulative_count + range_count)) / range_count;
            return percentile_value;
        }
    }

    return 0.0;
}

PG_FUNCTION_INFO_V1(ddsketch_init);
Datum ddsketch_init(PG_FUNCTION_ARGS) {
    DDSketch* sketch = create_ddsketch(0.02, 100);
    PG_RETURN_POINTER(sketch);
}

PG_FUNCTION_INFO_V1(ddsketch_add_value);
Datum ddsketch_add_value(PG_FUNCTION_ARGS) {
    DDSketch* sketch = (DDSketch*)PG_GETARG_POINTER(0);
    double value = PG_GETARG_FLOAT8(1);
    add_data_point(sketch, value);
    PG_RETURN_VOID();
}

PG_FUNCTION_INFO_V1(ddsketch_estimate_percentile);
Datum ddsketch_estimate_percentile(PG_FUNCTION_ARGS) {
    DDSketch* sketch = (DDSketch*)PG_GETARG_POINTER(0);
    double percentile = PG_GETARG_FLOAT8(1);
    double estimated_value = estimate_percentile(sketch, percentile);
    PG_RETURN_FLOAT8(estimated_value);
}

PG_FUNCTION_INFO_V1(ddsketch_destroy);
Datum ddsketch_destroy(PG_FUNCTION_ARGS) {
    DDSketch* sketch = (DDSketch*)PG_GETARG_POINTER(0);
    destroy_ddsketch(sketch);
    PG_RETURN_VOID();
}

PG_FUNCTION_INFO_V1(ddsketch_trigger_function);
Datum ddsketch_trigger_function(PG_FUNCTION_ARGS) {
    TriggerData* trigdata = (TriggerData*)fcinfo->context;
    TupleDesc tupdesc;
    HeapTuple tuple;
    bool isnull;
    double value;

    if (!CALLED_AS_TRIGGER(fcinfo))
        elog(ERROR, "ddsketch_trigger_function: not fired by trigger manager");

    if (!TRIGGER_FIRED_FOR_ROW(trigdata->tg_event))
        elog(ERROR, "ddsketch_trigger_function: must be fired for row");

    if (!TRIGGER_FIRED_BEFORE(trigdata->tg_event))
        elog(ERROR, "ddsketch_trigger_function: must be fired before event");

    DDSketch* sketch = (DDSketch*)PG_GETARG_POINTER(0);

    if (TRIGGER_FIRED_BY_INSERT(trigdata->tg_event))
    {
        tupdesc = trigdata->tg_relation->rd_att;
        tuple = trigdata->tg_trigtuple;

        value = DatumGetFloat8(SPI_getbinval(tuple, tupdesc, 1, &isnull));

        if (!isnull)
        {
            DirectFunctionCall2(ddsketch_add_value, PointerGetDatum(sketch), Float8GetDatum(value));
        }
    }

    if (TRIGGER_FIRED_BY_UPDATE(trigdata->tg_event))
    {
        tupdesc = trigdata->tg_relation->rd_att;
        tuple = trigdata->tg_newtuple;

        value = DatumGetFloat8(SPI_getbinval(tuple, tupdesc, 1, &isnull));

        if (!isnull)
        {
            DirectFunctionCall2(ddsketch_add_value, PointerGetDatum(sketch), Float8GetDatum(value));
        }
    }

    if (TRIGGER_FIRED_BY_DELETE(trigdata->tg_event))
    {
        /* Additional logic for deletion, if needed */
    }

    PG_RETURN_NULL();
}

