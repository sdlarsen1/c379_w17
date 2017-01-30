OBJS = test.o findpattern.o
CC = gcc
FLAGS = -m32 -std=c99
TITLE = -o output

all: $(OBJS)
	$(CC) $(FLAGS) $(OBJS) $(TITLE)

findpattern.o: findpattern.c findpattern.h
	$(CC) -c $(FLAGS) findpattern.c

test.o: test.c findpattern.h
	$(CC) -c $(FLAGS) test.c
