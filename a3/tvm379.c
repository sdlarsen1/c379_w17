#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "trace_files.h"
#include "page_table.h"
#include "tlb.h"


int main(int argc, char *argv[]) {

	if (argc < 8) {
		printf("Error, missing args!\n");
		return(1);
	}

	int pgsize = atoi(argv[1]);
	int tlbentries = atoi(argv[2]);
	char tlb_mode = *argv[3];
	int quantum = atoi(argv[4]);
	int physpages = atoi(argv[5]);
	char pt_mode = *argv[6];

	int num_tf = argc - 7;

	struct Trace_Files * trace_files = create_trace_files(num_tf);

	for (int i = 7; i < argc; i++) {
    	trace_files->file_ptrs[i] = fopen(argv[i], "r");
	}

	// create tlb and page table
	struct TLB * tlb = create_tlb(tlbentries, tlb_mode);
	struct Page_Table * page_table = create_page_table(physpages, pt_mode);

	bool done = false;
	int final_entry = quantum;
	do {
		for (int tf = 0; tf < (num_tf); tf++) {
			for (int offset = 0; offset < (final_entry * 4); offset += 4) {
				unsigned int pagenum = get_value_from_tf(trace_files, tf, offset);

				if (pagenum != 0) {
					if (query_entry_tlb(tlb, pagenum, (unsigned int) tf)) {
						trace_files->tlbhits[tf] += 1;  // tlbhit++ if exists
					} else {
						if (tlb_mode == 'p') {
							// flush tlb if not global
							flush_tlb(tlb);
						}

						if (!query_page_table(page_table, pagenum)) {  // not in page_table
							trace_files->pf[tf] += 1;
							if (add_entry_pt(page_table, pagenum, pt_mode)) {
								trace_files->pageout[tf] += 1;
							}
						}

						add_entry_tlb(tlb, pagenum, (unsigned int) tf);
					}
				} else {
					continue;
				}
				// update avgs
				double num_entries = count_entries(page_table, tf);
				trace_files = update_avs(trace_files, tf, num_entries);
			}
			final_entry += quantum;
		}
	} while (!done);

	// cleanup
	destroy_page_table(page_table);
	destroy_tlb(tlb);
	destroy_trace_files(trace_files, num_tf);

	for (int i; i < num_tf; i++) {
		int tlbhits = trace_files->tlbhits[i];
		int pf = trace_files->pf[i];
		int pageout = trace_files->pageout[i];
		double avs = get_avg(trace_files, i);

		printf("tlbhits: %d | pf: %d | pageout: %d | avs: %lf\n", tlbhits, pf, pageout, avs);
	}

}
