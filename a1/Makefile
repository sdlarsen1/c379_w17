OBJS = driver1.o findpattern.o
CC = gcc
FLAGS = -m32
SETPOS = -D pattern=$(p)
TITLE = -o output

all: $(OBJS)
	$(CC) $(FLAGS) $(OBJS) $(TITLE)

findpattern.o: findpattern.c findpattern.h
	$(CC) -c $(FLAGS) findpattern.c

driver1.o: driver1.c findpattern.h
	$(CC) -c $(FLAGS) driver1.c
test.o: test.c findpattern.h
	$(CC) -c $(FLAGS) test.c

clean:
	rm -rf output *.o
