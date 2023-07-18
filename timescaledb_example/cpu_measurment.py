import time
import psutil
import psycopg2

# Establish database connection
conn = psycopg2.connect(database="pgdatabase", host="/tmp", user="deykalion", port = "5432")
cursor = conn.cursor()

duration = 5  * 60  # Duration in seconds
end_time = time.time() + duration

while time.time() < end_time:
    cpu_percent = psutil.cpu_percent(interval=1)

    # Insert CPU usage into PostgreSQL
    query = "INSERT INTO cpu_usage (timestamp, usage) VALUES (now(), %s)"
    cursor.execute(query, (cpu_percent,))
    conn.commit()

    time.sleep(1)

query = "SELECT * FROM cpu_usage_avg_1m"
cursor.execute(query);
cursor.close()
conn.close()

print("Measurement done.")

