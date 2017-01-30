#include "findpattern.h"
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>

// need to add jumps or look for sigjmp to deal with
// infinite sigsegv loop

bool MEM_FAIL = false;

void segfault_handler(int sig) {
    MEM_FAIL = true;
}

unsigned int findpattern(unsigned char *pattern, unsigned int patlength,
     struct patmatch *locations, unsigned int loclength) {

    (void) signal(SIGSEGV, segfault_handler);  // temporary sig handler
    char mode;

    unsigned int *mem_pointer;
    unsigned long page;
    for (page = 0, mem_pointer = 0; page < MAX_ADDRESS;
        mem_pointer += getpagesize(), page += getpagesize()) {
        // check for valid mem location
        // if not, continue to next page
        // if valid, perform read/write test
        MEM_FAIL = false;
        int read;
        printf("%ld\n", page);

        read = *mem_pointer;  // read test
        printf("Got to read test\n");
        if (!MEM_FAIL) {
            *mem_pointer = 1;  // write test , if pass read test
            printf("Got to write test\n");
            if (!MEM_FAIL) {
                mode = MEM_RW;
            } else {
                mode = MEM_RO;
            }
        } else {
            continue;
        }
    }
    return 0;
}
