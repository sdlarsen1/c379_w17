#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <pthread.h>
#include <ctype.h>

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
int is_encrypted[WB_NUM_ENTRIES] = {0};	// 0 = unencrypted, 1 = encrypted
int entry_len[WB_NUM_ENTRIES] = {0};

pthread_t threads[MAX_THREADS];

pthread_mutex_t mutex;

// Function prototypes
void * handle_client(void * arg);

int handle_command(char * cmd, char * buffer);

int parse_cmd(char * cmd, char * type, int * entry, int * len, char **text);

int generate_message(int status, int entry, int length, char * buffer);

// Function definitions

int parse_cmd(char * cmd, char * type, int * entry, int * len, char **text)
{
	char * c;

	c = cmd;
	c++;	// skip first char

	if (!isdigit(*c))
	{
		// invalid command
		return -1;
	}

	*entry = atoi(c); // what entry does the command concern?


	while (isdigit(*c)) // next field
	{
		c++;
	}
	
	*type = *c;	// plain text or cipher?

	if (cmd[0] == '?')
	{
		// This is a query
		return 0;
	}
	
	else if ((*type != 'p') && (*type) != 'e')
	{
		// invalid command
		return -1;
	}
	c++;

	if (!isdigit(*c))
	{
		return -1;
	}

	*len = atoi(c);

	while (isdigit(*c))
	{
		c++;
	}
	c++;

	*text = c; // pointer to the text segment

	return 0;
}

int handle_command(char * cmd, char * buffer)
{
	int entry, len, valid;
	char type,  tmp[64] = {0};
	char * text;
	const char * err_noentry = "No Such Entry";
	const char * err_unknown_cmd = "Unknown Command";
	const char * err_toolong	= "Entry Too Long";

	valid = parse_cmd(cmd, &type, &entry, &len, &text);

	// start at entry 1

	if (valid < 0)
	{
		//unknown command
		sprintf(buffer, "!0e%d\n%s\n", strlen(err_unknown_cmd), err_unknown_cmd);
		return 0;
	}

	else if ((entry > WB_NUM_ENTRIES) || (entry <= 0))
	{
		// no such entry!
				strcat(buffer, "!");
		sprintf(tmp, "%d", entry);
		strcat(buffer, tmp);
		strcat(buffer, "e");
		sprintf(tmp, "%d", strlen(err_noentry));
		strcat(buffer, tmp);
		strcat(buffer, "\n");
		strcat(buffer, err_noentry);
		strcat(buffer, "\n\0");
		printf("%s", buffer);
	}

	else if (cmd[0] == '?')
	{
		printf("query\n");
		// query the whiteboard
		// -- start critical section --
		pthread_mutex_lock(&mutex);

		if (is_encrypted[entry - 1] == 0)
			type = 'p';
		else
			type = 'c';

		len = entry_len[entry - 1];
		sprintf(buffer, "!%d%c%d\n%s\n", entry, type, len, whiteboard[entry - 1]);

		pthread_mutex_unlock(&mutex);
		// -- exit critical section --
	}

	else if (cmd[0] == '@')
	{
		// update the whiteboard
		printf("update\n");
		if (len > (WB_ENTRY_SIZE - 1))
		{
			sprintf(buffer, "!%de%d\n%s\n", entry, 
					strlen(err_toolong), err_toolong);

			return 0;
		}

		// -- start critical section --
		pthread_mutex_lock(&mutex);

		memcpy(&(whiteboard[entry - 1]), text, len);
		whiteboard[entry - 1][len] = '\0';		

		entry_len[entry - 1] = len;

		if (type == 'p')
			is_encrypted[entry - 1] = 0;
		else
			is_encrypted[entry - 1] = 1;

		pthread_mutex_unlock(&mutex);
		// -- exit critical section -- 

		sprintf(buffer, "!%de0\n\n", entry);

	}

	return 0;
}


int main()
{
	int	sock, snew, fromlength;

	struct	sockaddr_in	master, from;

	char	in_buffer[BUFFER_LEN] = {0};
	char	out_buffer[BUFFER_LEN] = {0};

	const char * welcomeString = "CMPUT379 Whiteboard Server v0\n50\n";

	pthread_mutex_init(&mutex, NULL);

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

		//printf("Connection accepted\n");
		if (snew < 0) {
			perror ("Server: accept failed");
			exit (1);
		}

		//Send the welcome message to the new client
		send(snew, welcomeString, strlen(welcomeString) + 1, 0);
		// Spawn a thread for this new client
		

		//

		recv(snew, in_buffer, BUFFER_LEN, 0);
		printf("'%s'\n", in_buffer);

		handle_command(in_buffer, out_buffer);

		printf("%s", out_buffer);

		close (snew);
		//printf("Connection closed\n");
	}
}
