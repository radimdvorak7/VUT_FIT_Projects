CC=gcc				#GCC je stanoveny zadanim

FLAGS= -Wall -ggdb -g #-DNDEBUG
MODULES=ast code_gen data_types scanner semantic symtable vector parser error util code_gen_help scope_stack func_look_up

OBJS=$(addsuffix .o, $(MODULES))
MAIN=main.c
PROG=ifjcompiler

.PHONY: clean test unit_test zip

all: $(PROG)

unit_test:
	cmake -S test -B test/build && cd test/build && make && ./tests

test: $(TEST_BP)
	$(TEST_BP)

format:
	clang-tidy src/*

$(PROG): $(MAIN) $(OBJS)
	$(CC) $(FLAGS) $(MAIN) $(OBJS) -o $(PROG)

%.o: %.c %.h
	$(CC) -c $< $(FLAGS) -o $@

zip:
	zip xvacla37 Makefile *.c *.h rozdeleni rozsireni dokumentace.pdf

clean:
	rm -rf *.zip *.o $(PROG)
