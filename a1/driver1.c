#include <stdio.h>
#include <stdlib.h>
#include "findpattern.h"

int main(int argc, char *argv[]) {

    char *pattern = argv[0];
    int loclength = 16;
    struct patmatch *locations = malloc(sizeof(struct patmatch) * loclength);


    printf("Test1\n");
    printf("Memory modified on the heap using malloc()\n");

    printf("Pass 1\n");
    findpattern(pattern, sizeof(pattern), locations, loclength);

    // make changes

    printf("Pass 2\n");
    findpattern(pattern, sizeof(pattern), locations, loclength);
}
