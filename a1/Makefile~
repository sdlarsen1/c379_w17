OBJS = driver1.o findpattern.o
CC = gcc
FLAGS = -m32
SETPAT = -DP=$(P)
TITLE = -o output

all: $(OBJS)
	$(CC) $(FLAGS) $(OBJS) $(TITLE)

test2: driver2.o findpattern.o
	$(CC) $(FLAGS) driver2.o findpattern.o -o test2

test3: driver3.o findpattern.o
	$(CC) $(FLAGS) driver3.o findpattern.o -o test3

findpattern.o: findpattern.c findpattern.h
	$(CC) -c $(FLAGS) findpattern.c

driver1.o: driver1.c findpattern.h
	$(CC) -c $(FLAGS) driver1.c $(SETPAT)

driver2.o: driver2.c findpattern.h
	$(CC) -c $(FLAGS) driver2.c $(SETPAT)

driver3.o: driver3.c findpattern.h
	$(CC) -c $(FLAGS) driver3.c $(SETPAT)


test.o: test.c findpattern.h
	$(CC) -c $(FLAGS) test.c

clean:
	rm -rf output test2 test3 *.o
