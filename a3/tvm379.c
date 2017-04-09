#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "page_table.h"
#include "tlb.h"


int main(int argc, char *argv[]) {

	FILE * trace_files[argc-7];
	int * tlbhits;
	int * pf;

	if (argc < 8) {
		printf("Error, missing args!\n");
		return(1);
	}

	int pgsize = atoi(argv[1]);
	int tlbentries = atoi(argv[2]);
	char tlb_mode = argv[3];
	int quantum = atoi(argv[4]);
	int physpages = atoi(argv[5]);
	char pt_mode = argv[6];

	for (int i = 7; i < argc; i++) {
    	inputFiles[i] = fopen(argv[i], "r");
	}

	bool done = false;
	int final_entry = quantum;
	do {
		for (int i = 0; i < (argc-7); i++) {
			for (int j = 0; j < (counter * 4); j += 4) {
				if (query_entry()) {

				} else {
					// flush tlb if not global
					if (query_page_table()) {

					} else {
						add_entry_pt();
					}
					add_entry();
				}
			}
			final_entry += quantum;
		}
		// update values
	} while (!done);

}
