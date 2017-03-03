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

int send_buffer(int s, char * buffer, int buffer_len)
{
	int sent_bytes = 0;
	sent_bytes += write(s, buffer, buffer_len);
	while (sent_bytes < (buffer_len - 1))
	{
		sent_bytes += write(s, &buffer[sent_bytes], buffer_len - sent_bytes);
	}

	return 0;
}

int read_buffer(int s, char * buffer, int buffer_len)
{
	int read_bytes = 0;
	read_bytes = read (s, buffer, buffer_len);
	while (buffer[read_bytes] != "\0")
	{
		// WILL INFINITE LOOP IF MESSAGE NEVER COMPLETED
		read_bytes += read(s, &buffer[read_bytes], buffer_len - read_bytes);
	}
}
