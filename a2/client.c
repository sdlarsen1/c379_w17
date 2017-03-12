#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "encryption.h"

#define	MY_PORT  2222
#define BUFFER_LEN 1024
#define MESSAGE_LEN 512

const char *prepare_statement(char type, char *entry, char crypt, char *msg);
const char *get_user_input();


const char *get_server_response(char * in_buffer) {
	
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
			strcat(out_message, "e");  // encrypted
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

	// Add message
	if (msg != NULL) {
		strcat(out_message, msg);
		strcat(out_message, "\n");
	}

	// return/print out_message
	return out_message;
}


int main(int argc, char *argv[]) {
	int	s, number, got;

	struct	sockaddr_in	server;
	struct  in_addr		ip;
	struct	hostent		*host;

	char 		*ipstr;
	char		in_buffer[BUFFER_LEN];
	char	    out_buffer[BUFFER_LEN];

	if (argc == 1) {
		printf("No host specified, quitting\n");
		exit(0);
	}

	ipstr = argv[1];

	if (!inet_aton(ipstr, &ip)) {
		printf("Cant read IP\n");
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
		server.sin_port = htons (MY_PORT);

		if (connect (s, (struct sockaddr*) & server, sizeof (server))) {
			perror ("Client: cannot connect to server");
			exit (1);
		}

		recv(s, in_buffer, BUFFER_LEN, 0);
		fprintf (stdout, "%s", in_buffer);

		const char * temp_buff = get_user_input();
		memcpy(out_buffer, temp_buff, BUFFER_LEN);  // Copy formatted input to the out buffer

		send(s, out_buffer, strlen(out_buffer), 0);
		recv(s, in_buffer, BUFFER_LEN, 0);
		printf("Server's response:\n%s\n", get_server_response(in_buffer));

		close(s);
		memset(out_buffer, 0, BUFFER_LEN);  // Cleare the out buffer
	}

	recv(s, in_buffer, BUFFER_LEN, 0);

	close(s);
}
