#include <openssl/evp.h>
#include <openssl/pem.h>
#include <string.h>
#include <stdlib.h>

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


int main(){
    do_crypt();
    return 0;
}

int do_crypt()
{
    unsigned char outbuf[1024];
    unsigned char debuf[1024];
    int outlen, tmplen, delen, i;
    unsigned char key[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    unsigned char iv[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    EVP_CIPHER_CTX ctx;
    //unsigned char intext[] = "Mark Stevens is my friend for life man.";
    unsigned char intext[200];
    bzero(intext,200);
    char buf[200];
    sprintf(buf,"Enter a string to be encrypted\n");
    write(1,buf,strlen(buf));
    read(0,intext,199);
    intext[strlen(intext) - 1] = 0;
    EVP_CIPHER_CTX_init(&ctx);
    EVP_EncryptInit_ex(&ctx, EVP_aes_256_cbc(), NULL, key, iv);
    printf("\nintext is %d bytes in length\n\n",strlen(intext));
    if(!EVP_EncryptUpdate(&ctx, outbuf, &outlen, intext, strlen(intext)))
    {
        /* Error */
        return 0;
    }
    printf("** calling EVP_EncryptUpdate(&ctx, outbuf, &outlen, intext, strlen(intext))\n");
    printf("After EncryptUpdate, outlen = %d\n\n",outlen);
    /* Buffer passed to EVP_EncryptFinal() must be after data just
     * encrypted to avoid overwriting it.
     */
    if(!EVP_EncryptFinal_ex(&ctx, outbuf + outlen, &tmplen))
    {
        /* Error */
        return 0;
    }
    printf("** calling EVP_EncryptFinal_ex(&ctx, outbuf + outlen, &tmplen)\n");
    printf("After EncryptFinal, outlen = %d, tmplen = %d, ciphertext length is %d\n",outlen, tmplen, outlen + tmplen);
    printf("Padding is %d - %d = %d\n\n",outlen + tmplen, strlen(intext), (outlen + tmplen) - strlen(intext));
    outlen += tmplen;
    EVP_CIPHER_CTX_cleanup(&ctx);

    unsigned char * data_to_encode;  //The string we will base-64 encode.
    data_to_encode = malloc(outlen);
    memcpy(data_to_encode,outbuf,outlen);
    int bytes_to_encode = outlen; //Number of bytes in string to base64 encode.
    unsigned char *base64_encoded = base64encode(data_to_encode, bytes_to_encode);   //Base-64 encoding.

    int bytes_to_decode = strlen(base64_encoded); //Number of bytes in string to base64 decode.
    unsigned char *base64_decoded = base64decode(base64_encoded, bytes_to_decode);   //Base-64 decoding.

    outlen = strlen(base64_decoded);
    free(base64_encoded);                //Frees up the memory holding our base64 encoded data.
   // free(base64_decoded);                //Frees up the memory holding our base64 decoded data.
    EVP_CIPHER_CTX_init(&ctx);
    EVP_DecryptInit_ex(&ctx, EVP_aes_256_cbc(), NULL, key, iv);
    if(!EVP_DecryptUpdate(&ctx, debuf, &delen, base64_decoded, outlen))
    {
        /* Error */
        return 0;
    }
    printf("** calling EVP_DecryptUpdate(&ctx, debuf, &delen, base64_decoded, outlen)\n");
    printf("After DecryptUpdate, delen = %d\n\n",delen);
    int remainingBytes;
    if(!EVP_DecryptFinal_ex(&ctx, debuf + delen, &remainingBytes))
    {
        /* Error */
        return 0;
    }
    printf("** calling EVP_DecryptFinal_ex(&ctx, debuf + delen, &remainingBytes)\n");
    printf("After DecryptFinal, delen = %d, remainingBytes = %d\n",delen, remainingBytes);
    delen+=remainingBytes;
    EVP_CIPHER_CTX_cleanup(&ctx);



    return 0;
}
