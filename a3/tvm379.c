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
		FILE * file = fopen(argv[i], "r");
		printf("%p\n", file);
		if (!file) {
			printf("Error, unable to open file: %s\n", argv[i]);
			return 1;
		} else {
			trace_files->file_ptrs[i-7] = file;
		}
	}

	// create tlb and page table
	struct TLB * tlb = create_tlb(tlbentries, tlb_mode);
	struct Page_Table * page_table = create_page_table(physpages, pt_mode);

	bool done = false;
	int final_entry = quantum;
	int index_counter = 0;  // master index
	do {
		for (int tf = 0; tf < (num_tf); tf++) {
			int index = 0;  // temp index, for each trace file
			for (index += index_counter; index < final_entry; index ++) {

				unsigned int pagenum = get_value_from_tf(trace_files, tf, index);
				printf("This is the pagenum: %d\n", pagenum);

				if (pagenum != 0) {
					if (query_entry_tlb(tlb, pagenum, (unsigned int) tf+1)) {
						trace_files->tlbhits[tf] += 1;  // tlbhit++ if exists
					} else {
						if (tlb_mode == 'p') {
							flush_tlb(tlb);  // flush tlb if not global
						}

						if (!query_page_table(page_table, pagenum, tf+1)) {  // not in page_table
							trace_files->pf[tf] += 1;
							if (add_entry_pt(page_table, pagenum, pt_mode, tf+1)) {
								trace_files->pageout[tf] += 1;
							}
						}

						add_entry_tlb(tlb, pagenum, (unsigned int) tf+1);
					}
				} else {
					continue;
				}
				// update avgs
				double num_entries = count_entries(page_table, tf);
				update_avs(trace_files, tf, num_entries);

				if (index == num_tf-1) {
					index_counter += index;  // update the index_counter once done
				}
			}
		}

		final_entry += quantum;

		int count_done = 0;
		for (int i = 0; i < num_tf; i++) {
			if (feof(trace_files->file_ptrs[i])) {
				count_done ++;
			}
		}
		if (count_done == num_tf) {
			done = true;
		}

	} while (!done);

	// print results
	for (int i = 0; i < num_tf; i++) {
		int tlbhits = trace_files->tlbhits[i];
		int pf = trace_files->pf[i];
		int pageout = trace_files->pageout[i];
		double avs = get_avg(trace_files, i);

		printf("Trace File: %d | tlbhits: %d | pf: %d | pageout: %d | avs: %lf\n", i, tlbhits, pf, pageout, avs);
	}

	// cleanup
	destroy_page_table(page_table);
	destroy_tlb(tlb);
	destroy_trace_files(trace_files, num_tf);

}
