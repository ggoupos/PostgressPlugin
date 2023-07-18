DROP TABLE IF EXISTS cpu_usage CASCADE;
DROP MATERIALIZED VIEW IF EXISTS cpu_usage_avg_1m;

CREATE TABLE IF NOT EXISTS cpu_usage (
    timestamp TIMESTAMPTZ NOT NULL,
    usage NUMERIC NOT NULL
);

SELECT create_hypertable('cpu_usage', 'timestamp');

CREATE MATERIALIZED VIEW cpu_usage_avg_1m
WITH (timescaledb.continuous) AS
SELECT time_bucket('1 minute', timestamp) AS bucket,
       avg(usage) AS average_usage
FROM cpu_usage
GROUP BY bucket;
