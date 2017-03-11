#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/pem.h>

#define BUFFER_LEN 1024
#define MESSAGE_LEN 512


char *base64encode (const void *b64_encode_this, int encode_this_many_bytes){
    BIO *b64_bio, *mem_bio;      //Declares two OpenSSL BIOs: a base64 filter and a memory BIO.
    BUF_MEM *mem_bio_mem_ptr;    //Pointer to a "memory BIO" structure holding our base64 data.
    b64_bio = BIO_new(BIO_f_base64());                      //Initialize our base64 filter BIO.
    mem_bio = BIO_new(BIO_s_mem());                           //Initialize our memory sink BIO.
    BIO_push(b64_bio, mem_bio);            //Link the BIOs by creating a filter-sink BIO chain.
    BIO_set_flags(b64_bio, BIO_FLAGS_BASE64_NO_NL);  //No newlines every 64 characters or less.
    BIO_write(b64_bio, b64_encode_this, encode_this_many_bytes); //Records base64 encoded data.
    BIO_flush(b64_bio);   //Flush data.  Necessary for b64 encoding, because of pad characters.
    BIO_get_mem_ptr(mem_bio, &mem_bio_mem_ptr);  //Store address of mem_bio's memory structure.
    BIO_set_close(mem_bio, BIO_NOCLOSE);   //Permit access to mem_ptr after BIOs are destroyed.
    BIO_free_all(b64_bio);  //Destroys all BIOs in chain, starting with b64 (i.e. the 1st one).
    BUF_MEM_grow(mem_bio_mem_ptr, (*mem_bio_mem_ptr).length + 1);   //Makes space for end null.
    (*mem_bio_mem_ptr).data[(*mem_bio_mem_ptr).length] = '\0';  //Adds null-terminator to tail.
    return (*mem_bio_mem_ptr).data; //Returns base-64 encoded data. (See: "buf_mem_st" struct).
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


const char * do_decrypt(char * in_msg) {

    int bytes_to_decode = strlen(in_msg); //Number of bytes in string to base64 decode.
    int outlen, delen;
    static unsigned char out_msg[BUFFER_LEN];
    unsigned char *base64_decoded = base64decode(in_msg, bytes_to_decode);   //Base-64 decoding.
    unsigned char key[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    unsigned char iv[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    EVP_CIPHER_CTX ctx;

    outlen = strlen(base64_decoded);
    free(in_msg);                //Frees up the memory holding our base64 encoded data.

    EVP_CIPHER_CTX_init(&ctx);
    EVP_DecryptInit_ex(&ctx, EVP_aes_256_cbc(), NULL, key, iv);
    if(!EVP_DecryptUpdate(&ctx, out_msg, &delen, base64_decoded, outlen)) {
        /* Error */
        return 0;
    }

    int remainingBytes;
    if(!EVP_DecryptFinal_ex(&ctx, out_msg + delen, &remainingBytes)) {
        /* Error */
        return 0;
    }

    delen += remainingBytes;
    EVP_CIPHER_CTX_cleanup(&ctx);

    return out_msg, delen;
}

const char * do_crypt(char *in_msg) {

    static unsigned char out_msg[MESSAGE_LEN];
    int outlen, tmplen, i;
    unsigned char key[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    unsigned char iv[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    // char *in_msg = "some Crypto Text";
    EVP_CIPHER_CTX ctx;

    EVP_CIPHER_CTX_init(&ctx);
    EVP_EncryptInit_ex(&ctx, EVP_aes_256_cbc(), NULL, key, iv);

    if(!EVP_EncryptUpdate(&ctx, out_msg, &outlen, in_msg, strlen(in_msg))) {
        /* Error */
		printf("Error during encryption.");
        return 0;
    }

    /* Buffer passed to EVP_EncryptFinal() must be after data just
     * encrypted to avoid overwriting it.
     */
    if(!EVP_EncryptFinal_ex(&ctx, out_msg + outlen, &tmplen)) {
        /* Error */
		printf("Error while encrypting final block.");
        return 0;
    }

    outlen += tmplen;
    EVP_CIPHER_CTX_cleanup(&ctx);

	return base64encode(out_msg, outlen);
}


const char * prepare_statement(char type, char *entry, char crypt, char *msg) {
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
			strcat(out_message, "p");  // plain_msg
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

const char * encrypt_msg(char *msg) {
	return 0;
}

int main() {
	char type;
	char crypt[2];
	char entry[4];
	char msg[MESSAGE_LEN];
	char *crypto_msg;

	printf("What type of query do you wish to make?\n(1) GET\n(2) POST\n>");
	type = fgetc(stdin);

	printf("Which entry do you wish to query?\n>");
	scanf("%s", entry);

	if (type == '2') {
		printf("Do you wish to use encryption?\n(1) Yes \n(2) No\n>");
		scanf("%s", crypt);

		printf("What is you update message?\n>");
		scanf("%s", msg);

		if (crypt[0] == '1') {
			crypto_msg = do_crypt(msg);
			printf("%s", prepare_statement(type, entry, crypt[0], crypto_msg));
		} else {
			printf("%s", prepare_statement(type, entry, crypt[0], msg));
		}
	} else {
		printf("%s", prepare_statement(type, entry, crypt[0], msg));
	}
}
