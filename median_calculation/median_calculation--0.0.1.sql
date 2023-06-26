CREATE OR REPLACE FUNCTION median_calculation(text, text) RETURNS float 
    AS 'MODULE_PATHNAME', 'median_calculation'
    LANGUAGE C STRICT;
