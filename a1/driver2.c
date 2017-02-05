#include <stdio.h>
#include <stdlib.h>
#include "findpattern.h"
#include <string.h>


#define MIN(x, y) (((x) < (y)) ? (x) : (y))

// https://gcc.gnu.org/onlinedocs/cpp/Stringification.html
#define STRINGIFY(s) XSTRINGIFY(s)
#define XSTRINGIFY(s) #s
#ifndef P
#define P ""
#endif

/*
int call_findpattern(char *pattern, int patlength,
    struct patmatch *locations, int loclength) {

    




   // *new_instance = *pattern;
    //*new_instance2 = *pattern;

    int patterns_found = findpattern(pattern, patlength, locations, loclength);
    return patterns_found;
}
*/

int main(int argc, char *argv[]) {

    char *pattern = STRINGIFY(P);
    char new_instance[100] = {0};
    char new_instance2[100] = {0};

    int loclength = 16, patlength, patterns_found, i;
    struct patmatch *locations = malloc(sizeof(struct patmatch) * loclength);
    struct patmatch *loc;
    patlength = strlen(pattern);
    printf("Test2\n");
    printf("Memory modified on the stack using local variables\n");
    printf("The pattern is: %s\n", pattern);

    printf("'Pattern var at %.8x'\n", (unsigned int) pattern);

    printf("\nPass 1\n");
 
    patterns_found = findpattern(pattern, patlength, locations, loclength);
    printf("Found %d instances of the pattern.\n", patterns_found);
    // print out where we found the pattern.
    for (loc = locations, i = 0; i < MIN(loclength, patterns_found); i++, loc++) {
        printf("Pattern found at %.8x, ", loc->location);
        if (loc->mode == 0) {
            printf("Mode: MEM_RW\n");
        } else {
            printf("Mode: MEM_RO\n");
        }
    }

    strcpy(new_instance, pattern);
    printf("\nNew instance of pattern put at %.x\n", (unsigned int) new_instance);

    strcpy(new_instance2, pattern);
    printf("New instance of pattern put at %.x\n", (unsigned int) new_instance2);

    // start 2nd pass
    printf("\nPass 2\n");
    // patterns_found = findpattern(pattern, patlength, locations, loclength);
    patterns_found = findpattern(pattern, patlength, locations, loclength);
    printf("Found %d instances of the pattern.\n", patterns_found);

    // print out where we found the pattern.
    for (loc = locations, i = 0; i < MIN(loclength, patterns_found); i++, loc++) {
        printf("Pattern found at %.8x, ", loc->location);
        if (loc->mode == 0) {
            printf("Mode: MEM_RW\n");
        } else {
            printf("Mode: MEM_RO\n");
        }
    }
    printf("pattern length: %d\n", patlength);

    return 0;
}
