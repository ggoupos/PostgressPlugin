# Makefile for median_calculation extension

MODULE_big = median_calculation
OBJS = median_calculation.o

EXTENSION = median_calculation

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)

CFLAGS += -I /usr/include/postgresql/server/postgres.h


