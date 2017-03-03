#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

int send_buffer(int snew, char * buffer, int buffer_len);

int read_buffer(int s, char * buffer, int buffer_len);

