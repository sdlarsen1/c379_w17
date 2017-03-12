#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include "encryption.h"

const char * do_decrypt_new(char * in_msg, char *key) {

    int outlen, delen;
    static unsigned char out_msg[MESSAGE_LEN], out_buff[MESSAGE_LEN];
    // unsigned char key[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    unsigned char iv[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    EVP_CIPHER_CTX ctx;

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


const char * do_crypt_new(char *in_msg, char *key) {

    static unsigned char out_msg[MESSAGE_LEN];
    int outlen, tmplen, i;
    // unsigned char key[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    unsigned char iv[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    // char *in_msg = "some Crypto Text";
    EVP_CIPHER_CTX ctx;

    printf("Before encryption, in_msg = %s\n", in_msg);

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
    printf("After encryption, out_msg = %s\n", base64encode(out_msg, outlen));
	return base64encode(out_msg, outlen);
}

int main() {
    char file_key[] = "ezAsMSwyLDMsNCw1LDYsNyw4LDksMTAsMTEsMTIsMTMsMTQsMTUsMCwxLDIsMyw0LDUsNiw3LDgsOSwxMCwxMSwxMiwxMywxNCwxNX0=";
    char *key = base64decode(file_key, strlen(file_key));
    printf("%s", key);
}
