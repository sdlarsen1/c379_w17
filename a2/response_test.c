#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <openssl/evp.h>
#include <openssl/pem.h>


#define BUFFER_LEN 1024
#define MESSAGE_LEN 512


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


char *base64decode (const void *b64_decode_this, int decode_this_many_bytes){
    BIO *b64_bio, *mem_bio;      //Declares two OpenSSL BIOs: a base64 filter and a memory BIO.
    char *base64_decoded = calloc( (decode_this_many_bytes*3)/4+1, sizeof(char) ); //+1 = null.
    b64_bio = BIO_new(BIO_f_base64());                      //Initialize our base64 filter BIO.
    mem_bio = BIO_new(BIO_s_mem());                         //Initialize our memory source BIO.
    BIO_write(mem_bio, b64_decode_this, decode_this_many_bytes); //Base64 data saved in source.
    BIO_push(b64_bio, mem_bio);          //Link the BIOs by creating a filter-source BIO chain.
    BIO_set_flags(b64_bio, BIO_FLAGS_BASE64_NO_NL);          //Don't require trailing newlines.
    int decoded_byte_index = 0;   //Index where the next base64_decoded byte should be written.
    while ( 0 < BIO_read(b64_bio, base64_decoded+decoded_byte_index, 1) ){ //Read byte-by-byte.
        decoded_byte_index++; //Increment the index until read of BIO decoded data is complete.
    } //Once we're done reading decoded data, BIO_read returns -1 even though there's no error.
    BIO_free_all(b64_bio);  //Destroys all BIOs in chain, starting with b64 (i.e. the 1st one).
    return base64_decoded;        //Returns base-64 decoded data with trailing null terminator.
}


const char * do_decrypt(char * in_msg, char *encoded_key) {

    int outlen, delen;
    static unsigned char out_msg[MESSAGE_LEN], out_buff[MESSAGE_LEN];
    // unsigned char key[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    unsigned char iv[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    unsigned char *key;
    EVP_CIPHER_CTX ctx;

    // Decode the key
    key = base64decode(encoded_key, strlen(encoded_key));

    int bytes_to_decode = strlen(in_msg);  // Number of bytes in string to base64 decode.
    unsigned char *base64_decoded = base64decode(in_msg, bytes_to_decode);   // Base-64 decoding.

    outlen = strlen(base64_decoded);

    EVP_CIPHER_CTX_init(&ctx);
    EVP_DecryptInit_ex(&ctx, EVP_aes_256_cbc(), NULL, key, iv);
    if(!EVP_DecryptUpdate(&ctx, out_buff, &delen, base64_decoded, outlen)) {
        /* Error */
        printf("Error during decryption.\n");
        return 0;
    }

    int remainingBytes;
    if(!EVP_DecryptFinal_ex(&ctx, out_buff + delen, &remainingBytes)) {
        /* Error */
        printf("Error while decrypting the final block.\n");
        return 0;
    }

    delen += remainingBytes;
    EVP_CIPHER_CTX_cleanup(&ctx);

    strncpy(out_msg, out_buff, delen);  // Remove extra padding

    return out_msg;
}


void get_server_response(char * in_buffer, char **keys, int *line_count) {
    char *pch, *e, *entry, *msg_len;
    int index;
    bool is_encrypted = false;

    pch = strtok(in_buffer, "\n");

    while (pch != NULL)
    {
        if (pch[0] == '!') {  // Deal with header
            printf("Header: %s\n", pch);

            c = strchr(pch, 'c');  // index the e, set encryption status is exists
            if (c) { is_encrypted = true; }

        } else if (is_encrypted) {

            bool decrypted = false;
            printf("Trying first key...\n");
            for (i = 0; i < line_count; i++) {
                char *msg = do_decrypt_new(pch, keys[i]);
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


int main(int argc, char* argv[]) {
    char response1[] = "!12p30\nthisisaresponsetodemothelength\n";
    char response2[] = "!14c44\nGMVZ0Bdr7x9fF1XUNyj4gFUKsHwd4SWF57LUUJXyQ9U=\n";

    int line_count;
    char *filename = argv[1];
    FILE* keyfile = fopen(filename, "r");
    if (keyfile == NULL) {
        printf("Can't open file %s.\n", filename);
        return 1;
    }

    char **keys = get_keys(keyfile, &line_count);
    fclose(keyfile);

    get_server_response(response1, keys, &line_count);
    get_server_response(response2, keys, &line_count);

    // Clean-up
    for (i = 0; i < line_count; i++) {
        free(keys[i]);
    }
    free(keys);
}
