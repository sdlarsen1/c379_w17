CC = gcc
FLAGS = -m32 -std=c99

all:
	make tvm379

tvm379: tvm379.o page_table.o tlb.o
	$(CC) $(FLAGS) tvm379.o page_table.o tlb.o -o tvm379

page_table.o: page_table.c page_table.h
	$(CC) -c $(FLAGS) page_table.c

tlb.o: tlb.c tlb.h
	$(CC) -c $(FLAGS) tlb.c

tvm379.o: tvm379.c tvm379.h tlb.o page_table.o
	$(CC) $(FLAGS) tvm379.c tlb.o page_table.o

clean:
	rm -rf *.o tvm379
