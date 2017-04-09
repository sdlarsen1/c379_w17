#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "page_table.h"
#include "tlb.h"


struct Trace_Files {
	FILE * file_ptrs;
	int * tlbhits;
	int * pf;
	int * pageout;
};


struct Trace_Files * create_trace_files(int num_tf) {
	struct Trace_Files * trace_files;
	trace_files->file_ptrs = malloc((sizeof FILE) * (num_tf));
	trace_files->tlbhits = malloc((sizeof int) * num_tf);
	trace_files->pf = malloc((sizeof int) * num_tf);
	trace_files->pageout = malloc((sizeof int) * num_tf);

	return trace_files;
}


int main(int argc, char *argv[]) {

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

	int num_tf = argc - 7;

	struct Trace_Files * trace_files = create_trace_files(num_tf);

	for (int i = 7; i < argc; i++) {
    	trace_files->file_ptrs[i] = fopen(argv[i], "r");
	}

	// Create tlb and page table
	struct TLB * tlb = create_tlb(tlbentries, tlb_mode);
	struct Page_Table * page_table = create_page_table(physpages, pt_mode);

	bool done = false;
	int final_entry = quantum;
	do {
		for (int i = 0; i < (num_tf); i++) {
			for (int j = 0; j < (final_entry * 4); j += 4) {
				int pagenum = trace_files->file_ptrs[i][j]
				if (query_entry(tlb, pagenum)) {
					trace_files->tlbhits[i] += 1;
				} else {
					if (mode == 'p') {
						// flush tlb if not global
					}

					if (!query_page_table(page_table, pagenum)) {  // not in page_table
						trace_files->pf[i] += 1;
						if (add_entry_pt(page_table, pagenum, mode)) {
							trace_files->pageout[i] += 1;
						}
					}

					add_entry();
				}
			}
			final_entry += quantum;
		}
		// update avs?
	} while (!done);

	// print+format values

}
