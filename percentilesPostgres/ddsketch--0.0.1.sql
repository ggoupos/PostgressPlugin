CREATE EXTENSION IF NOT EXISTS ddsketch;

CREATE FUNCTION ddsketch_init() RETURNS ddsketch AS 'MODULE_PATHNAME', 'ddsketch_init' LANGUAGE C STRICT;
CREATE FUNCTION ddsketch_add_value(sketch ddsketch, value FLOAT8) RETURNS VOID AS 'MODULE_PATHNAME', 'ddsketch_add_value' LANGUAGE C STRICT;
CREATE FUNCTION ddsketch_estimate_percentile(sketch ddsketch, percentile FLOAT8) RETURNS FLOAT8 AS 'MODULE_PATHNAME', 'ddsketch_estimate_percentile' LANGUAGE C STRICT;
CREATE FUNCTION ddsketch_destroy(sketch ddsketch) RETURNS VOID AS 'MODULE_PATHNAME', 'ddsketch_destroy' LANGUAGE C STRICT;

CREATE FUNCTION ddsketch_trigger_function() RETURNS TRIGGER AS 'MODULE_PATHNAME', 'ddsketch_trigger_function' LANGUAGE C STRICT;

CREATE TRIGGER compute_ddsketch_trigger
AFTER INSERT OR UPDATE ON my_table
FOR EACH ROW
EXECUTE FUNCTION ddsketch_trigger_function();

CREATE TABLE IF NOT EXISTS ddsketch_table (sketch ddsketch);
INSERT INTO ddsketch_table VALUES (ddsketch_init());
