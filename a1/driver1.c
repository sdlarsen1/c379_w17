#include <stdio.h>
#include <stdlib.h>
#include "findpattern.h"
#include <string.h>


#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#ifndef P
#define P ""
#endif

int main(int argc, char *argv[]) {

    char *pattern = P;
    int loclength = 16, patlength, patterns_found, i;
    struct patmatch *locations = malloc(sizeof(struct patmatch) * loclength);
    struct patmatch *loc;
    patlength = strlen(pattern);

    printf("Test1\n");
    printf("Memory modified on the heap using malloc()\n");

    printf("Pass 1\n");
    // The size of pattern is always the same   >!!!!!!!< this is a pointer
    patterns_found = findpattern(pattern, patlength, locations, loclength);
    printf("Found %d instances of the pattern.\n", patterns_found);
    // make changes

    char c = '1';
    
    printf("Pass 2\n");
    patterns_found = findpattern(pattern, patlength, locations, loclength);
    printf("Found %d instances of the pattern.\n", patterns_found);

    // print out where we found the pattern.
    for (loc = locations, i = 0; i < MIN(loclength, patterns_found); i++, loc++)
    {
	printf("Pattern found at %.8x, ", loc->location);
	if (loc->mode == 0)
		printf("Mode: MEM_RW\n");
	else
		printf("Mode: MEM_RO\n");
    }
    printf("pattern length: %d\n", patlength);
	
}
