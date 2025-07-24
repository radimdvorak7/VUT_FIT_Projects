# Řešení IJC-DU1, 26.03.2024
# Autor: Radim Dvořák, xdvorar00, FIT
LC_ALL= cs_CZ.utf8
CC= gcc
M32FLAG= #-m32
OPTFLAG= #-O2
CFLAGS= $(OPTFLAG) -g -std=c11 -pedantic -Wall -Wextra $(M32FLAG)
LDLIBS= -lm $(M32FLAG)
TARGET= primes primes-i no-comment

#CFLAGS += -fsanitize=address
#LDFLAGS += -fsanitize=address

.PHONY: all zip clean run

all: $(TARGET)

run: all
	ulimit -s 82000 && ./primes
	ulimit -s 82000 && ./primes-i

primes: primes.o eratosthenes.o error.o

primes-i: primes-i.o eratosthenes-i.o error.o bitset-i.o

no-comment:	error.o

%-i.o: %.c
	$(CC) -DUSE_INLINE $(CFLAGS) -c -o $@ $<

clean:
	rm *.o $(TARGET)

zip:
	zip xdvorar00.zip *.c *.h Makefile
