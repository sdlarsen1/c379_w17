#include "encryption.h"

#define MESSAGE_LEN 512


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

    int outlen, delen;
    static unsigned char out_msg[MESSAGE_LEN], out_buff[MESSAGE_LEN];
    unsigned char key[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
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


int main() {
    char in_msg[] = "kPnQZQVkVmsS2fCfUJY6rxS3bRnqlyr9OSqVuLEb3gU=";
    printf("After decryption: %s\n", do_decrypt(in_msg));
}
