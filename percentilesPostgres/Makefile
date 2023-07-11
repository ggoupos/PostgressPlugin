#Makefile for postgress_parallax extension

MODULES = ddsketch 
EXTENSION = ddsketch 
OBJS = ddsketch.o 
DATA = ddsketch--0.0.1.sql


PG_CONFIG = /usr/local/pgsql/bin/pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)


