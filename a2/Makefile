CC = gcc
FLAGS = -m32 -pthread -lcrypto

all:
	make client server

client: client.o
	$(CC) $(FLAGS) client.o -o client

server: server.o
	$(CC) $(FLAGS) server.o -o server

server.o: server.c
	$(CC) -c $(FLAGS) server.c

client.o: client.c
	$(CC) -c $(FLAGS) client.c

clean:
	rm -rf server client *.o
