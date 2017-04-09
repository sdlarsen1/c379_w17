CC = gcc
FLAGS = -m32

all:
	make tvm379

tvm379: tvm379.o page_table.o tlb.o
	$(CC) $(FLAGS) tvm379.o page_table.o tlb.o -o tvm379

page_table.o: page_table.c page_table.h
	$(CC) -c $(FLAGS) page_table.c page_table.h

tlb.o: tlb.c tlb.h
	$(CC) -c $(FLAGS) tlb.c tlb.h

tvm379.o: tvm379.c tvm379.h
	$(CC) $(FLAGS) tvm379.c tvm379.h

clean:
	rm -rf *.o tvm379
