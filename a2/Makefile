CC = gcc
FLAGS = -m32

all:
	make client server

client: client.o
	$(CC) $(FLAGS) client.o -o client

server: server.o
	$(CC) $(FLAGS) server.o -o server

server.o: client.c
	$(CC) -c $(FLAGS) client.c

client.o: server.c
	$(CC) -c $(FLAGS) server.c

clean:
	rm -rf server client *.o
