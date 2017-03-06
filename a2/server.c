#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
//#include "connection.h"

#define	MY_PORT	2222
#define BUFFER_LEN 512
/* ---------------------------------------------------------------------
 This	is  a sample server which opens a stream socket and then awaits
 requests coming from client processes. In response for a request, the
 server sends an integer number  such	 that  different  processes  get
 distinct numbers. The server and the clients may run on different ma-
 chines.
 --------------------------------------------------------------------- */

/*
Tips:
	- each client/connection has its own thread
	- all threads share the same globals (ie. the whiteboard and mutex), each thread gets its own stack
	- mutex isn't the whiteboard itself, separate variable/array/file/etc.
	- server only shuts down after given SIGTERM
	- basic error handling (index out of range, syntax, etc.)
	- server may run with statefile to preserve data after crash (statefile going to be used to test servers)
	- specify input in README
*/


int main()
{
	int	sock, snew, fromlength, number, outnum;

	struct	sockaddr_in	master, from;

	char	in_buffer[BUFFER_LEN];
	char	out_buffer[BUFFER_LEN];

	const char * welcomeString = "\nWelcome to 379 whiteboard!\n";

	sock = socket (AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		perror ("Server: cannot open master socket");
		exit (1);
	}

	master.sin_family = AF_INET;
	master.sin_addr.s_addr = INADDR_ANY;
	master.sin_port = htons (MY_PORT);

	if (bind (sock, (struct sockaddr*) &master, sizeof (master))) {
		perror ("Server: cannot bind master socket");
		exit (1);
	}

	number = 0;

	listen (sock, 5);

	while (1) {
		fromlength = sizeof (from);
		snew = accept (sock, (struct sockaddr*) & from, & fromlength);
		printf("Connection accepted\n");
		if (snew < 0) {
			perror ("Server: accept failed");
			exit (1);
		}
		//outnum = htonl (number);

		send(snew, welcomeString, strlen(welcomeString) + 1, 0);

		// Spawn a thread for this new client

		//
		recv(snew, in_buffer, BUFFER_LEN, 0);
		printf("%s\n", in_buffer);

		close (snew);
		printf("Connection closed\n");
		// number++;

	}
}
