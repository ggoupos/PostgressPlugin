#Makefile for postgress_parallax extension

MODULES = postgress_parallax
EXTENSION = postgress_parallax
OBJS = postgress_parallax.o 
DATA = postgress_parallax--0.0.1.sql
REGRESS = postgress_parallax_test

PG_LDFLAGS += -L/home/deykalion/ITE/parallax_carv/parallax/build/install/usr/local/lib -lparallax 

PG_CONFIG = /usr/local/pgsql/bin/pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)


