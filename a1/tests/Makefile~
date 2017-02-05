CC = gcc
FLAGS = -m32
SETPAT = -Dp=$(p)

all:
	make test1 test2 test3
	./test1 >> test_results.txt
	./test2 >> test_results.txt
	./test3 >> test_results.txt

clean:
	rm -rf *.o ./test1 ./test2 ./test3
	rm test_results.txt

test1:  driver1.o findpattern.o
	$(CC) $(FLAGS) driver1.o findpattern.o -o test1

test2:  driver2.o findpattern.o
	$(CC) $(FLAGS) driver2.o findpattern.o -o test2

test3:  driver3.o findpattern.o
	$(CC) $(FLAGS) driver3.o findpattern.o -o test3

driver1.o: ../findpattern.o driver1.c
	$(CC) -c $(FLAGS) $(SETPAT) driver1.c

driver2.o: ../findpattern.o driver2.c
	$(CC) -c $(FLAGS) $(SETPAT) driver2.c

driver3.o: ../findpattern.o driver3.c
	$(CC) -c $(FLAGS) $(SETPAT) driver3.c

findpattern.o: ../findpattern.h ../findpattern.c
	$(CC) -c $(FLAGS) ../findpattern.c
