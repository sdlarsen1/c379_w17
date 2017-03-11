#include "encryption.h"

#define MESSAGE_LEN 512


int main() {
    char in_msg[] = "kPnQZQVkVmsS2fCfUJY6rxS3bRnqlyr9OSqVuLEb3gU=";
    printf("After decryption: %s\n", do_decrypt(in_msg));
}
