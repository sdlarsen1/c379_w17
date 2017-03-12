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

const char *prepare_statement(char type, char *entry, char crypt, char *msg);
const char *get_user_input();


void get_server_response(char * in_buffer) {
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
            printf("Message before decryption: %s\n", pch);
            printf("Message: %s\n", do_decrypt(pch));
        } else {
            printf("Message: %s\n", pch);
        }
        pch = strtok(NULL, "\n");  // Get next token
    }
}


const char *get_user_input() {

	char type[2];
	char crypt[2];
	char entry[4];
	char msg[MESSAGE_LEN];
	char *crypto_msg;

	printf("What type of query do you wish to make?\n(1) GET\n(2) POST\n(3) Clear an entry\n(4) Terminate and dump statefile\n>");
	scanf("%s", type);

	printf("Which entry do you wish to query?\n>");
	scanf("%s", entry);


	if (type[0] == '1') {  // GET query

		memset(msg, 0, MESSAGE_LEN);  // Clear msg
		return prepare_statement(type[0], entry, crypt[0], msg);

	} else if (type[0] == '2') {

		printf("Do you wish to use encryption?\n(1) Yes\n(2) No\n>");
		scanf("%s", crypt);

		printf("What is your update message?\n>");
		scanf("%s", msg);

		if (crypt[0] == '1') {  // POST query
			crypto_msg = do_crypt(msg);
			return prepare_statement(type[0], entry, crypt[0], crypto_msg);
		} else {
			return prepare_statement(type[0], entry, crypt[0], msg);
		}

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

	if (type == '2') {
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
	}

	strcat(out_message, "\n");

	if (crypt == '1') {
		strcat(out_message, "CMPUT379 Whiteboard Encrypted v0\n");
	}

	// Add message
	if (msg != NULL) {
		strcat(out_message, msg);
		strcat(out_message, "\n");
	}

	// return/print out_message
	return out_message;
}


int main(int argc, char *argv[]) {
	int	s, number, got, port;

	struct	sockaddr_in	server;
	struct  in_addr		ip;
	struct	hostent		*host;

	char 		*ipstr, *filename;
	char		in_buffer[BUFFER_LEN];
	char	    out_buffer[BUFFER_LEN];

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
	} else {
		printf("WARNING: No keyfile specified -- unable to decrypt encrypted entries.\n")
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

		const char * temp_buff = get_user_input();
		memcpy(out_buffer, temp_buff, BUFFER_LEN);  // Copy formatted input to the out buffer
		printf("This is the out message: %s",out_buffer);
		send(s, out_buffer, strlen(out_buffer), 0);

		recv(s, in_buffer, BUFFER_LEN, 0);
		get_server_response(in_buffer);  // NEEDS TO BE PROPERLY TESTED

		close(s);
		memset(out_buffer, 0, BUFFER_LEN);  // Cleare the out buffer
	}

	recv(s, in_buffer, BUFFER_LEN, 0);

	close(s);
}
