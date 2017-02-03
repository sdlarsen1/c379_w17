OBJS = driver1.o findpattern.o
CC = gcc
FLAGS = -m32
SETPAT = -DP=$(P)
TITLE = -o output

all: $(OBJS)
	$(CC) $(FLAGS) $(OBJS) $(TITLE)

findpattern.o: findpattern.c findpattern.h
	$(CC) -c $(FLAGS) findpattern.c

driver1.o: driver1.c findpattern.h
	$(CC) -c $(FLAGS) -E driver1.c $(SETPAT) 
test.o: test.c findpattern.h
	$(CC) -c $(FLAGS) test.c

clean:
	rm -rf output *.o
