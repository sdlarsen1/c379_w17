#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include "encryption.h"

#define BUFFER_LEN 1024
#define MESSAGE_LEN 512


const char *prepare_statement(char type, char *entry, char crypt, char *msg) ;
const char *get_user_input(char *key) ;


/*
    Code taken from
    http://stackoverflow.com/questions/22863977/dynamically-allocated-2d-array-of-strings-from-file
*/
char ** get_keys(FILE* keyfile, int *count) {
    char** array = NULL;        /* Array of lines */
    int    i;                   /* Loop counter */
    char   line[256];           /* Buffer to read each line */
    int    line_count;          /* Total number of lines */
    int    line_length;         /* Length of a single line */


    /* Get the count of lines in the file */
    line_count = 0;
    while (fgets(line, sizeof(line), keyfile) != NULL) {
       line_count++;
    }

    /* Move to the beginning of file. */
    rewind(keyfile);

    /* Allocate an array of pointers to strings
     * (one item per line). */
    array = malloc(line_count * sizeof(char *));
    if (array == NULL) {
        return NULL; /* Error */
    }

    /* Read each line from file and deep-copy in the array. */
    for (i = 0; i < line_count; i++) {
        /* Read the current line. */
        fgets(line, sizeof(line), keyfile);

        /* Remove the ending '\n' from the read line. */
        line_length = strlen(line);
        line[line_length - 1] = '\0';
        line_length--; /* update line length */

        /* Allocate space to store a copy of the line. +1 for NUL terminator */
        array[i] = malloc(line_length + 1);

        /* Copy the line into the newly allocated space. */
        strcpy(array[i], line);
    }

    /* Return the array of lines */
    *count = line_count;
    return array;
}


void get_server_response(char * in_buffer, char **keys, int *line_count) {
    char *pch, *c, *entry, *msg_len;
    int index;
    bool is_encrypted = false;

    pch = strtok(in_buffer, "\n");

    while (pch != NULL)
    {
        if (pch[0] == '!') {  // Deal with header
            printf("Header: %s\n", pch);

            c = strchr(pch, 'c');  // index the c, set encryption status is exists
            if (c) { is_encrypted = true; }

        } else if (is_encrypted) {

            char *msg;
            bool decrypted = false;

            printf("Trying first key...\n");

            int i;
            for (i = 0; i < *line_count; i++) {  // try each key, break on success
                msg = do_decrypt(pch, keys[i]);
                if (!msg) {
                    printf("Trying new key...\n");
                } else {
                    decrypted = true;
                    break;
                }
            }
            if (!decrypted) {
                printf("Unable to decrypt the message.\n");
            } else {
                printf("Message: %s", msg);
            }

        } else {
            printf("Message: %s\n", pch);
        }
        pch = strtok(NULL, "\n");  // Get next token
    }
}

const char *get_user_input(char *key) {

	char type[2];
	char crypt[2];
	char entry[4];
	char msg[MESSAGE_LEN];
	char *crypto_msg;

	printf("What type of query do you wish to make?\n(1) GET\n(2) POST\n(3) Clear an entry\n>");
	scanf("%s", type);

	printf("Which entry do you wish to query?\n>");
	scanf("%s", entry);


	if (type[0] == '1') {  // GET query

		memset(msg, 0, MESSAGE_LEN);  // Clear msg
		return prepare_statement(type[0], entry, crypt[0], msg);

	} else if (type[0] == '2') {  // POST query

		if (key != "none") {
			printf("Do you wish to use encryption?\n(1) Yes\n(2) No\n>");
			scanf("%s", crypt);
		} else { strcpy(crypt, "2"); }

		printf("What is your update message?\n>");
		scanf("%s", msg);

		if (crypt[0] == '1') {  // Add encryption macro + encrypt
			char macro_msg[] = "CMPUT379 Whiteboard Encrypted v0\n";
			strcat(macro_msg, msg);
			crypto_msg = do_crypt(macro_msg, key);
			return prepare_statement(type[0], entry, crypt[0], crypto_msg);
		} else {
			return prepare_statement(type[0], entry, crypt[0], msg);
		}

	} else {
		return prepare_statement(type[0], entry, crypt[0], msg);
	}
}


const char *prepare_statement(char type, char *entry, char crypt, char *msg) {

	static char out_message[BUFFER_LEN];

	if (type == '1') {
		strcpy(out_message, "?");  // GET
	} else {
		strcpy(out_message, "@");  // POST
	}

	strcat(out_message, entry);

	if (type != '1') {
		if (crypt == '1') {
			strcat(out_message, "c");  // encrypted
		} else {
			strcat(out_message, "p");  // plaintext
		}
	}

	// Add length of msg here
	if (type == '2') {
		int length = strlen(msg);
		char buff[sizeof(int)];
		sprintf(buff, "%d", length);
		strcat(out_message, buff);
	} else if (type == '3') {
		strcat(out_message, "0");
	}

	strcat(out_message, "\n");

	// Add message
	if (type == '2') {
		strcat(out_message, msg);
	}
	strcat(out_message, "\n");

	// return/print out_message
	return out_message;
}


int main(int argc, char *argv[]) {
	int	s, number, got, port, line_count;

	struct	sockaddr_in	server;
	struct  in_addr		ip;
	struct	hostent		*host;

	char 		*ipstr, *filename;
	char		in_buffer[BUFFER_LEN];
	char	    out_buffer[BUFFER_LEN];
	char		**keys;
	bool		has_keys;

	if (argc == 1) {
		printf("Missing args, quitting\n");
		exit(0);
	}

	ipstr = argv[1];
	port = atoi(argv[2]);

	if (argc == 4) {
		filename = argv[3];
		FILE* keyfile = fopen(filename, "r");
		if (keyfile == NULL) {
        	printf("Can't open file %s.\n", filename);
			exit(0);
    	}
		keys = get_keys(keyfile, &line_count);
		if (keys) { has_keys = true; }
		else { has_keys = false; }
		fclose(keyfile); // close file, don't need it anymore
	} else {
		printf("WARNING: No keyfile specified -- unable to use encryption.\n");
	}


	if (!inet_aton(ipstr, &ip)) {
		printf("Can't read IP\n");
		exit(0);
	}

	host = gethostbyaddr ((const void *) &ip, sizeof(ip), AF_INET);

	printf("Trying to connect to %s ...\n", argv[1]);

	if (host == NULL) {
		perror ("Client: cannot get host description");
		exit (1);
	}




	while(1) {
		s = socket (AF_INET, SOCK_STREAM, 0);

		if (s < 0) {
			perror ("Client: cannot open socket");
			exit (1);
		}

		bzero (&server, sizeof (server));
		bcopy (host->h_addr, &(server.sin_addr), host->h_length);
		server.sin_family = host->h_addrtype;
		server.sin_port = htons (port);

		if (connect (s, (struct sockaddr*) & server, sizeof (server))) {
			perror ("Client: cannot connect to server");
			exit (1);
		}

		recv(s, in_buffer, BUFFER_LEN, 0);
		fprintf (stdout, "%s", in_buffer);
		memset(in_buffer, 0, BUFFER_LEN);

		if (has_keys == false) { strcpy(keys[0], "none"); }
		const char * temp_buff = get_user_input(keys[0]);
		memcpy(out_buffer, temp_buff, BUFFER_LEN);  // Copy formatted input to the out buffer
		// printf("This is the out message: %s",out_buffer);
		send(s, out_buffer, strlen(out_buffer), 0);

		recv(s, in_buffer, BUFFER_LEN, 0);
		// printf("This is the in_buffer: %s", in_buffer);
		get_server_response(in_buffer, keys, &line_count);  // NEEDS TO BE PROPERLY TESTED

		close(s);
		memset(out_buffer, 0, BUFFER_LEN);  // Cleare the out buffer
	}

	recv(s, in_buffer, BUFFER_LEN, 0);

	close(s);

	// Clean-up
    int i;
    for (i = 0; i < line_count; i++) {
        free(keys[i]);
    }
    free(keys);
}
