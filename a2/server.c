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

#define BUFFER_LEN 512
#define WB_ENTRY_SIZE 1024
#define MAX_ENTRIES 10000
#define MAX_THREADS 20
#define WB_NUM_ENTRIES 50 //default number of entries

/*
	TO DO:
		Have the welcome string dynamically change depending on NUM_ENTRIES
		Interpretation of statefiles
		Multi-threading
*/

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

int NUM_ENTRIES = WB_NUM_ENTRIES;
char **whiteboard;	// array of pointers to strings, init in main
int *is_encrypted;
int *entry_len;

//char whiteboard[WB_NUM_ENTRIES][WB_ENTRY_SIZE] = {0};
//int is_encrypted[WB_NUM_ENTRIES] = {0};	// 0 = unencrypted, 1 = encrypted
//int entry_len[WB_NUM_ENTRIES] = {0};

pthread_t threads[MAX_THREADS];

pthread_mutex_t mutex;

// Function prototypes
void * handle_client(void * arg);

int handle_command(char * cmd, char * buffer);

int parse_cmd(char * cmd, char * type, int * entry, int * len, char **text);

int count_entries(FILE * file);


int main(int argc, char * argv[])
{
	// declare vars
	int	sock, snew, fromlength, i, statefileflag = 0, portnum;

	struct	sockaddr_in	master, from;

	char	in_buffer[BUFFER_LEN] = {0};
	char	out_buffer[BUFFER_LEN] = {0};
	char * statefilename = NULL;

	const char * welcomeString = "CMPUT379 Whiteboard Server v0\n50\n";

	FILE * statefile;

	pthread_mutex_init(&mutex, NULL);

	portnum = atoi(argv[1]);

	// -- init whiteboard--
	for (i = 2; i < argc; i++)
	{

		if (strcmp(argv[i], "-n") == 0)
		{
			NUM_ENTRIES = atoi(argv[i+1]);

			if ((NUM_ENTRIES > 10000) || (NUM_ENTRIES < 1))
			{
				printf("Error: number of entries must be between 1 and %d", MAX_ENTRIES);
				exit(0);
			}
		}
		if (strcmp(argv[i], "-f") == 0) 
		{
			statefilename = argv[i+1];
			statefileflag = 1;
		}
	}

	if (statefileflag)
	{
		printf("Statefile = '%s'\n", statefilename);
		statefile = fopen(statefilename, "r");

		if (statefile == NULL)
		{
			perror("Could not open statefile");
			exit(0);
		}

		NUM_ENTRIES = count_entries(statefile);
		fclose(statefile);
	}

	printf("num entries == %d\n",NUM_ENTRIES);

	// allocate space for the whiteboard
	whiteboard = (char **) malloc(NUM_ENTRIES * sizeof(char *));
	for (i = 0; i < NUM_ENTRIES; i++)
	{
		whiteboard[i] = malloc(WB_ENTRY_SIZE * sizeof(char));
	}

	is_encrypted = (int *) malloc(NUM_ENTRIES * sizeof(int));
	entry_len = (int *) malloc(NUM_ENTRIES * sizeof(int));

	// act on statefile


	// -- init socket --
	sock = socket (AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		perror ("Server: cannot open master socket");
		exit (1);
	}

	master.sin_family = AF_INET;
	master.sin_addr.s_addr = INADDR_ANY;
	master.sin_port = htons (portnum);

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
		// removed  & \/
		memcpy(whiteboard[entry - 1], text, len);
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

int count_entries(FILE * file)
{
	int i, entries = 0, len;
	char c[10];
	char ch;

	while(!feof(file))
	{
		if (fgetc(file) == '!')		// start of entry
		{
			entries++;
			while (isdigit(fgetc(file)))
				;
			fgetc(file);
			ch = fgetc(file);

			for (i = 0; isdigit(ch); i++)
			{
				c[i] = ch;
				ch = fgetc(file);
			}

			c[i+1] = '\0';

			len = atoi(c);
			for (i = 0; i < (len + 1); i++)
				fgetc(file);
		}
		
	}

	return entries;
}
