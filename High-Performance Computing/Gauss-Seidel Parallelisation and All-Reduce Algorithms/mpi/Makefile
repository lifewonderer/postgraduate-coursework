CC = mpicc
CFLAGS = -Wall -Wextra
CCFLAGS =
OBJ = reduce.o utils.o

.PHONY: clean all

all: main

main: $(OBJ) reduce.h utils.h Makefile

main: main.c
	$(CC) $(CFLAGS) $(CCFLAGS) -o $@ $< $(OBJ)

reduce.o: reduce.c reduce.h utils.h Makefile

utils.o: utils.c utils.h Makefile

%.o: %.c
	$(CC) $(CFLAGS) $(CCFLAGS) -c -o $@ $<

clean:
	-rm -f main reduce.o utils.o
