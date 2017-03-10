#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>

#define BUFFER_LEN 1024
#define MESSAGE_LEN 512

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

    // for (i=0; i<=(outlen-1); i++){
	// 	printf("%C", out_msg[i]);
	// }

	// printf("\n");
    // return 1;
	return out_msg;
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
