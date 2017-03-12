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
#include <signal.h>

#define BUFFER_LEN 512
#define WB_ENTRY_SIZE 1024
#define MAX_ENTRIES 10000
#define MAX_THREADS 20
#define WB_NUM_ENTRIES 50 //default number of entries

/*
	TO DO:
		cleanup
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

//pthread_t threads[MAX_THREADS];
//int thread_free[MAX_THREADS] = {0};

pthread_mutex_t mutex;
pthread_mutex_t log_mutex;

FILE * logfp = NULL;

// Function prototypes
void * handle_client(void * arg);

void sigterm_handler(int sig);

int handle_command(char * cmd, char * buffer);

int parse_cmd(char * cmd, char * type, int * entry, int * len, char **text);

int count_entries(FILE * file);

void loadcmd(char * buffer, FILE * statefile);

int getfreethread(void);

int main(int argc, char * argv[])
{
	//-- daemonify --
	pid_t pid = 0;
    	pid_t sid = 0;

    	pid = fork();

    	if (pid < 0)
    	{
        	printf("fork failed!\n");
        	exit(1);
    	}

    	if (pid > 0)
	{
    	// in the parent
       		printf("pid of server daemon %d \n", pid);
       		exit(0);
    	}

    	umask(0);

	// open a log file
	// logfp is global
    	logfp = fopen ("server.log", "w+");
    	if(!logfp){
    		printf("cannot open log file");
    	}

	//printf("I am a daemon\n");
	// create new process group

    	sid = setsid();
    	if(sid < 0)
    	{
    		fprintf(logfp, "cannot create new process group");
        	exit(1);
    	}

	/* Change the current working directory 
    	if ((chdir("/")) < 0) {
      		printf("Could not change working directory to /\n");
      		exit(1);
    	} */

	close(STDIN_FILENO);
    	//close(STDOUT_FILENO);
    	close(STDERR_FILENO);

	signal(SIGTERM,sigterm_handler);

	fprintf(logfp, "server start\n");


	// declare vars
	int	sock, snew, fromlength, i, statefileflag = 0, portnum, th;
	pthread_t newthread;

	struct	sockaddr_in	master, from;

	char	in_buffer[BUFFER_LEN] = {0};
	char	out_buffer[BUFFER_LEN] = {0};
	char	tmp[2 * BUFFER_LEN];
	char * statefilename = NULL;

	char welcomeString[128];

	FILE * statefile;

	pthread_mutex_init(&mutex, NULL);
	pthread_mutex_init(&log_mutex, NULL);

	portnum = atoi(argv[1]);

	// -- init whiteboard--
	for (i = 2; i < argc; i++)
	{

		if (strcmp(argv[i], "-n") == 0)
		{
			NUM_ENTRIES = atoi(argv[i+1]);

			if ((NUM_ENTRIES > 10000) || (NUM_ENTRIES < 1))
			{
				fprintf(logfp, "Error: number of entries must be between 1 and %d", 												MAX_ENTRIES);
				fclose(logfp);
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
		// printf("Statefile = '%s'\n", statefilename);
		statefile = fopen(statefilename, "r");

		if (statefile == NULL)
		{
			fprintf(logfp, "Could not open statefile");
			fclose(logfp);
			exit(0);
		}

		NUM_ENTRIES = count_entries(statefile);
		fclose(statefile);
	}

	fprintf(logfp, "num entries == %d\n",NUM_ENTRIES);
	

	// allocate space for the whiteboard
	whiteboard = (char **) malloc(NUM_ENTRIES * sizeof(char *));
	for (i = 0; i < NUM_ENTRIES; i++)
	{
		whiteboard[i] = malloc(WB_ENTRY_SIZE * sizeof(char));
	}

	is_encrypted = (int *) malloc(NUM_ENTRIES * sizeof(int));
	entry_len = (int *) malloc(NUM_ENTRIES * sizeof(int));

	// act on statefile
	if (statefileflag)
	{
		statefile = fopen(statefilename, "r");
		for (i = 0; i < NUM_ENTRIES; i++ )
		{
			loadcmd(tmp, statefile);
			handle_command(tmp, out_buffer);
		}
		fclose(statefile);
	}

	sprintf(welcomeString,"CMPUT379 Whiteboard v0\n%d\n", NUM_ENTRIES);	

	// -- init socket --
	sock = socket (AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		fprintf(logfp, "Server: cannot open master socket");
		fclose(logfp);
		exit (1);
	}

	master.sin_family = AF_INET;
	master.sin_addr.s_addr = INADDR_ANY;
	master.sin_port = htons (portnum);

	if (bind (sock, (struct sockaddr*) &master, sizeof (master))) {
		fprintf(logfp, "Server: cannot bind master socket");
		fclose(logfp);
		exit (1);
	}

	listen (sock, 5);
	
	while (1) {
		fromlength = sizeof (from);

		snew = accept (sock, (struct sockaddr*) & from, & fromlength);

		//printf("Connection accepted\n");
		if (snew < 0) {
			//logfp = fopen("server.log", "w+");
			//if (logfp)
			//	exit(1);
			pthread_mutex_lock(&log_mutex);
			fprintf(logfp, "Server: accept failed");

			fclose(logfp);			
			exit (1);
		}

		//Send the welcome message to the new client
		send(snew, welcomeString, strlen(welcomeString) + 1, 0);
		// Spawn a thread for this new client
		//th = getfreethread();
	
		pthread_create(&newthread, NULL, handle_client, (void *) &snew);
		pthread_mutex_lock(&log_mutex);
		fprintf(logfp, "created new thread: %d\n", (int) (unsigned long) newthread);
		pthread_mutex_unlock(&log_mutex);				

		//

		//recv(snew, in_buffer, BUFFER_LEN, 0);
		//fprintf(logfp, "received '%s'\n", in_buffer);

		//handle_command(in_buffer, out_buffer);

		//fprintf(logfp, "response '%s'\n", out_buffer);
		//send(snew, out_buffer, strlen(out_buffer)+ 1, 0);
		//close (snew);

		//fflush(logfp);
		//printf("Connection closed\n");
	}
}

// Function definitions
void * handle_client(void * arg)
{
	int socket = *(int *) arg;
	char	in_buffer[BUFFER_LEN] = {0};
	char	out_buffer[BUFFER_LEN] = {0};

	// enter command loop
	while (recv(socket, in_buffer, BUFFER_LEN, 0) > 0)
	{
		printf("%s\n", in_buffer);
		handle_command(in_buffer, out_buffer);
		send(socket, out_buffer, strlen(out_buffer)+ 1, 0);
		memset(in_buffer, 0, BUFFER_LEN * sizeof(char));
		memset(out_buffer, 0, BUFFER_LEN * sizeof(char));
	}

	close(socket);

	// free the thread
	pthread_mutex_lock(&log_mutex);
	fprintf(logfp, "thread closed: %d\n", (int) (unsigned long)  pthread_self());
	pthread_mutex_unlock(&log_mutex);

	pthread_exit(0);
}

/*
int getfreethread(void)
{
	int i;
	for (i = 0; i < MAX_THREADS; i++)
	{
		if (thread_free[i] == 0)
			return i;	// index of a free thread
	}

	return -1;
}
*/

void sigterm_handler(int sig){
	fprintf(logfp, "received SIGTERM\n");
	
	pthread_mutex_lock(&log_mutex);
	int i;
	FILE * statefile;
	char query[16], output[2 * WB_ENTRY_SIZE];
	

	statefile = fopen("whiteboard.all", "w");
	if (!statefile)
	{
		fprintf(logfp, "could not open whiteboard.all, dump failed\n");
		fclose(logfp);
		exit(1);
	}

	// dump to statefile
	for (i = 0; i < NUM_ENTRIES; i++)
	{
		sprintf(query, "?%d", i + 1);
		handle_command(query, output);
		fprintf(statefile, "%s", output);
		free(whiteboard[i]);
	}

	free(whiteboard);
	free(is_encrypted);
	free(entry_len);

	fclose(statefile);

	fprintf(logfp, "dump to statefile success\n");
	
	fclose(logfp);
    	exit(0);
}

void loadcmd(char * buffer, FILE * statefile)
{
	char ch;

	int i = 0, j, len = 0;
	buffer[0] = '@';
	i++;

	fgetc(statefile);	// skip '!'
	ch = fgetc(statefile);

	while(isdigit(ch))
	{
		buffer[i] = ch;		// entry num
		i++;
		ch = fgetc(statefile);
	}


	buffer[i] =  ch;	// p or c
	i++;
	ch = fgetc(statefile);
	while(isdigit(ch))  // length
	{
		buffer[i] = ch;
		i++;

		len = 10 * len + (ch - '0');   // convert length to int
		ch = fgetc(statefile);
	}

	buffer[i] = ch;	// '\n'
	i++;
	
	for (j = 0; j < len; j++)	// the plain / ciphertext
	{
		buffer[i] = fgetc(statefile);
		i++;
	}

	buffer[i] = fgetc(statefile);	// '\n'
	i++;
	buffer[i] = '\0';

	return;
}

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
	
	else if ((*type != 'p') && (*type) != 'c')
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
	}

	else if (cmd[0] == '?')
	{
		//printf("query\n");
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
		// printf("update\n");
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
