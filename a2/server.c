#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <pthread.h>

#define	MY_PORT	2222
#define BUFFER_LEN 512
#define WB_NUM_ENTRIES 50
#define WB_ENTRY_SIZE 1024
#define MAX_THREADS 20

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

// persistent connections, server itself is a daemon

// GLOBALS

char whiteboard[WB_NUM_ENTRIES][WB_ENTRY_SIZE] = {0};

pthread_t threads[MAX_THREADS];



// Function prototypes
void * handle_client(void * arg);

int handle_command(char * cmd);

int parse_entry(char * cmd);


// Function definitions

int handle_command(char cmd[])
{
	int entry, length;
	char type;

	if (cmd[0] == '?')
	{
		// query the whiteboard
		;
	}
	else if (cmd[0] == '@')
	{
		// update the whiteboard
		;
	}
	else
	{
		//unknown command
		;
	}

	return 0;
}


int main()
{
	int	sock, snew, fromlength;

	struct	sockaddr_in	master, from;

	char	in_buffer[BUFFER_LEN];
	char	out_buffer[BUFFER_LEN];

	const char * welcomeString = "CMPUT379 Whiteboard Server v0\n50\n";


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

	listen (sock, 5);

	while (1) {
		fromlength = sizeof (from);

		snew = accept (sock, (struct sockaddr*) & from, & fromlength);

		printf("Connection accepted\n");
		if (snew < 0) {
			perror ("Server: accept failed");
			exit (1);
		}

		//Send the welcome message to the new client
		send(snew, welcomeString, strlen(welcomeString) + 1, 0);
		// Spawn a thread for this new client
		

		//

		recv(snew, in_buffer, BUFFER_LEN, 0);
		printf("%s\n", in_buffer);

		close (snew);
		printf("Connection closed\n");
	}
}
