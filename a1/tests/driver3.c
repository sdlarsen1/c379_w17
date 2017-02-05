#include <stdio.h>
#include <stdlib.h>
#include "../findpattern.h"
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

// https://gcc.gnu.org/onlinedocs/cpp/Stringification.html
#define STRINGIFY(s) XSTRINGIFY(s)
#define XSTRINGIFY(s) #s
#ifndef p
#define p ""
#endif


int main(int argc, char *argv[]) {

    char *pattern = STRINGIFY(p);
    char *filename = "driver3file";
    FILE *fp;
    int fd;
    void *fileregion;
    int loclength = 16, patlength, patterns_found, i;
    struct patmatch *locations = malloc(sizeof(struct patmatch) * loclength);
    struct patmatch *loc;
    patlength = strlen(pattern);

    printf("\n\n----< Test3 >----\n\n");
    printf("Memory modified using mmap / mprotect().\n");
    printf("The pattern is: %s\n", pattern);

    printf("Pass 1\n");
  
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


    // start 2nd pass
    printf("\nPass 2\n");
    fp = fopen(filename, "w");
    fprintf(fp, "%s", pattern);

    // find size of file
    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    printf("File size: %d bytes\n", size);
    fclose(fp);

    // do mmap
    fd = open(filename, O_RDWR);

    fileregion = mmap(0, size, PROT_WRITE, MAP_SHARED, fd, 0);  
    printf("File MMAP'd to: %.x\n", (unsigned int) fileregion);
      
    
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

    // start 3rd pass

    mprotect(fileregion, size, PROT_READ);
    printf("\nPass 3\n");
    printf("MMAP'd memory at %.x set to read-only.\n", (unsigned int) fileregion);

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

    return 0;
}
