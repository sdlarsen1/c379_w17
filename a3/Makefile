CC = gcc
FLAGS = -m32 -std=c99

all:
	make tvm379

tvm379: tvm379.c page_table.o tlb.o trace_files.o
	$(CC) $(FLAGS) tvm379.c page_table.o tlb.o trace_files.o -o tvm379

page_table.o: page_table.c page_table.h
	$(CC) -c $(FLAGS) page_table.c

tlb.o: tlb.c tlb.h
	$(CC) -c $(FLAGS) tlb.c

trace_files.o: trace_files.c trace_files.h
	$(CC) -c $(FLAGS) trace_files.c

clean:
	rm -rf *.o tvm379 *.h.gch
