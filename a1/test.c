#include "findpattern.h"
#include <stdlib.h>

int main(void) {
    struct patmatch *testpattern = malloc(sizeof(struct patmatch));
    char *test = malloc(sizeof(char));

    findpattern(test, 3, testpattern, 1);
}
