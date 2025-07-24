# Řešení IJC-DU2, 25.04.2024
# Autor: Radim Dvořák, xdvorar00, FIT

LC_ALL= cs_CZ.utf8

CC= gcc
CXX= g++

DCFLAGS= -fPIC
CFLAGS= -g -O2 -std=c11 -pedantic -Wall -Wextra
CXXFLAGS= -std=c++17 -pedantic -Wall

STATIC_LIB=libhtab.a
DYNAMIC_LIB=libhtab.so
PROGS= tail wordcount wordcount-dynamic wordcount-

.PHONY: $(PROGS) $(DYNAMIC_LIB) $(STATIC_LIB) run zip clean
#CFLAGS += -fsanitize=address 
#LDFLAGS += -fsanitize=address

export LD_LIBRARY_PATH=$(PWD)

all:$(PROGS)

tail: tail.o

MODULES=htab_bucket_count.o\
		htab_clear.o\
		htab_erase.o\
		htab_find.o\
		htab_for_each.o\
		htab_free.o\
		htab_hash_function.o\
	    htab_init.o\
		htab_lookup_add.o\
		htab_size.o\
		htab_statistics.o
	    
$(filter %.o,$(MODULES)): %.o: %.c
	$(CC) -c $(CFLAGS) $(DCFLAGS)  $< -o $@

$(STATIC_LIB): $(MODULES)
	ar crs $@ $^

$(DYNAMIC_LIB): $(MODULES)
	$(CC) -shared $(DCFLAGS) -o $@ $^

wordcount: io.o wordcount.o $(STATIC_LIB)
	$(CC) $(CFLAGS) -o $@ -Bstatic io.o wordcount.o -L. -lhtab

wordcount-dynamic: io.o wordcount.o $(DYNAMIC_LIB)
	$(CC) $(CFLAGS) $(DCFLAGS) -o $@ io.o wordcount.o -L. -lhtab

wordcount-:wordcount-.o
	$(CXX) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

run:$(PROGS)
	./wordcount
	export LD_LIBRARY_PATH=. && ./wordcount-dynamic

clean:
	rm -f *.o $(PROGS) $(MODULES) $(STATIC_LIB) $(DYNAMIC_LIB)

zip:
	zip xdvorar00.zip *.c *.cc *.h Makefile
