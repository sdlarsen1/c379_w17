CC = gcc
FLAGS = -m32

all:
	make client server

client: client.o connection.o
	$(CC) $(FLAGS) client.o connection.o -o client

server: server.o connection.o
	$(CC) $(FLAGS) server.o connection.o -o server

server.o: server.c
	$(CC) -c $(FLAGS) server.c 

client.o: client.c
	$(CC) -c $(FLAGS) client.c

connection.o: connection.c connection.h
	$(CC) -c $(FLAGS) connection.c

clean:
	rm -rf server client *.o
