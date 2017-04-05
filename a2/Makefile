CC = gcc
FLAGS = -m32 -pthread

all:
	make client server

client: client.o encryption.o
	$(CC) client.o encryption.o $(FLAGS) -L//usr/lib/x86_64-linux-gnu/libcrypto.so -lcrypto -o wbc379

server: server.o
	$(CC) $(FLAGS) server.o -o wbs379

server.o: server.c
	$(CC) -c $(FLAGS) server.c

client.o: client.c
	$(CC) -c $(FLAGS) client.c -lcrypto 

encryption.o: encryption.c encryption.h
	$(CC) -c encryption.c -lcrypto

clean:
	rm -rf server client wbs379 wbc379 *.o
