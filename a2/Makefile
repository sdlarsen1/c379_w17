CC = gcc
FLAGS = -m32 -pthread

all:
	make client server

client: client.o
	$(CC) $(FLAGS) client.o -o wbc379

server: server.o
	$(CC) $(FLAGS) server.o -o wbs379

server.o: server.c
	$(CC) -c $(FLAGS) server.c 

client.o: client.c
	$(CC) -c $(FLAGS) client.c

clean:
	rm -rf server client *.o
