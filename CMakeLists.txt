cmake_minimum_required(VERSION 3.12)
project(continuous_aggregates_project)

# Find PostgreSQL libraries
find_package(PostgreSQL REQUIRED)

# Add the executable for database setup
add_executable(database_setup src/database_setup.c)
target_link_libraries(database_setup ${PostgreSQL_LIBRARIES})

# Add the executable for data population
add_executable(data_population src/data_population.c)
target_link_libraries(data_population ${PostgreSQL_LIBRARIES})

# Add the executable for continuous aggregates
add_executable(continuous_aggregates src/continuous_aggregates.c)
target_link_libraries(continuous_aggregates ${PostgreSQL_LIBRARIES})

# Add the executable for test refresh
add_executable(test_refresh src/test_refresh.c)
target_link_libraries(test_refresh ${PostgreSQL_LIBRARIES})

