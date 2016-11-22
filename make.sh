#!/bin/sh

PRFX=
# PRFX=/usr/local/pgsql/

gcc -std=c99 -I$PRFX/include/ -c libpq_example.c -o libpq_example.o
gcc -std=c99 -L$PRFX/lib/ libpq_example.o -Wl,-Bstatic -lpq -Wl,-Bdynamic $(pg_config --libs) -o libpq_example
