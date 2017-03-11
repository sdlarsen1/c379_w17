#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/pem.h>

#define MESSAGE_LEN 512

// Function prototypes
char *base64encode (const void *b64_encode_this, int encode_this_many_bytes) ;
char *base64decode (const void *b64_decode_this, int decode_this_many_bytes) ;
const char * do_decrypt(char * in_msg) ;
const char * do_crypt(char *in_msg) ;