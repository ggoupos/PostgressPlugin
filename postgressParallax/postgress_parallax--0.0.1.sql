CREATE OR REPLACE FUNCTION postgress_parallax(text, text, text) RETURNS text 
    AS 'MODULE_PATHNAME', 'postgress_parallax'
    LANGUAGE C STRICT;
