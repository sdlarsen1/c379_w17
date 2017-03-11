CC = gcc
FLAGS = -m32 -pthread -lcrypto

all:
	make client server

client: client.o encryption.o
	$(CC) $(FLAGS) client.o encryption.o -o client

server: server.o
	$(CC) $(FLAGS) server.o -o server

server.o: server.c
	$(CC) -c $(FLAGS) server.c

client.o: client.c encryption.o
	$(CC) -c $(FLAGS) client.c

encryption.o: encryption.c encryption.h
	$(CC) -c $(FLAGS) encryption.c

clean:
	rm -rf server client *.o
