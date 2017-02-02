#include <stdio.h>
#include <stdlib.h>
#include "findpattern.h"

int main(int argc, char *argv[]) {

    char *pattern = argv[0];
    struct patmatch *locations = malloc(sizeof(struct patmatch));


    printf("Test1\n");
    printf("Memory modified on the heap using malloc()\n");

    printf("Pass 1\n");
    findpattern(pattern, sizeof(pattern), locations, 16);

    // make changes

    printf("Pass 2\n");
    findpattern(pattern, sizeof(pattern), locations, 16);
}
