/* Header for encryption.c */

#define MESSAGE_LEN 512

char *base64encode (const void *b64_encode_this, int encode_this_many_bytes) ;
char *base64decode (const void *b64_decode_this, int decode_this_many_bytes) ;
const char * do_decrypt(char * in_msg, char *encoded_key) ;
const char * do_crypt(char *in_msg, char *encoded_key) ;
