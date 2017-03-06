#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "connection.h"

#define	MY_PORT  2222
#define BUFFER_LEN 1024

/* ---------------------------------------------------------------------
 This is a sample client program for the number server. The client and
 the server need not run on the same machine.
 --------------------------------------------------------------------- */

int main(int argc, char *argv[])
{
	int	s, number, got;

	struct	sockaddr_in	server;
	struct  in_addr		ip;
	struct	hostent		*host;

	char 	*ipstr;
	char	in_buffer[BUFFER_LEN];
	char	out_buffer[BUFFER_LEN];

	if (argc == 1) {
		printf("No host specified, quitting\n");
		exit(0);
	}

	ipstr = argv[1];

	if (!inet_aton(ipstr, &ip))
	{
		printf("Cant read IP\n");
		exit(0);
	}

	host = gethostbyaddr ((const void *) &ip, sizeof(ip), AF_INET);

	printf("Trying to connect to %s ...\n", argv[1]);

	if (host == NULL) {
		perror ("Client: cannot get host description");
		exit (1);
	}

	while (1) {

		s = socket (AF_INET, SOCK_STREAM, 0);

		if (s < 0) {
			perror ("Client: cannot open socket");
			exit (1);
		}

		bzero (&server, sizeof (server));
		bcopy (host->h_addr, &(server.sin_addr), host->h_length);
		server.sin_family = host->h_addrtype;
		server.sin_port = htons (MY_PORT);

		//printf("ip addr: %s\n server addr: %s\n", host.h_addr, server.sin_addr)

		if (connect (s, (struct sockaddr*) & server, sizeof (server))) {
			perror ("Client: cannot connect to server");
			exit (1);
		}

		// read (s, &number, sizeof (number));
		got = read(s, in_buffer, BUFFER_LEN);
		close (s);
		printf("got %d bytes\n", got);
		fprintf (stderr, "Process %d gets string %s\n", getpid (),
			in_buffer);
		sleep (2);
	}
}
