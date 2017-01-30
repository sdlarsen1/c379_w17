OBJS = test.o findpattern.o
CC = gcc
FLAGS = -std=c99 -m32
TITLE = -o output

all: $(OBJS)
	$(CC) $(FLAGS) $(OBJS) $(TITLE)

findpattern.o: findpattern.c findpattern.h
	$(CC) -c $(FLAGS) findpattern.c

test.o: test.c findpattern.h
	$(CC) -c $(FLAGS) test.c
