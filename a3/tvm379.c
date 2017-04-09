#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "tvm379.h"
#include "page_table.h"
#include "tlb.h"


struct Trace_Files * create_trace_files(int num_tf) {
	struct Trace_Files * trace_files;

	trace_files = (struct Trace_Files *) malloc(sizeof(struct Trace_Files));
	trace_files->file_ptrs = (FILE *) malloc((sizeof(FILE *)) * (num_tf));
	trace_files->tlbhits = (int) malloc((sizeof(int)) * num_tf);
	trace_files->pf = (int) malloc((sizeof(int)) * num_tf);
	trace_files->pageout = (int) malloc((sizeof(int)) * num_tf);
	trace_file->avs = (double *) malloc((sizeof(double *)) * num_tf);

	for (int i = 0; i < num_tf; i++) {
		trace_files->avs[i] = (double) malloc(sizeof(double));
	}

	return trace_files;
}


void destroy_trace_files(struct Trace_Files * trace_files, int num_tf) {
	free(trace_files->file_ptrs);
	free(trace_files->tlbhits);
	free(trace_files->pf);
	free(trace_files->pageout);

	for (int i=0; i < num_tf, i++) {
		free(trace_files->avs[i]);
	}

	free(trace_files->avs);
	free(trace_files);
}


void update_avs(struct Trace_Files ** trace_files, int tf, double value) {  // pass in &trace_files
	int avs_length = sizeof(trace_file->avs[tf]) / sizeof(double);

	trace_files->avs[tf] = (double) realloc(trace_files->avs[tf], sizeof(double) * (avs_length+1));  // realloc memory

	trace_files->avs[tf][avs_length+1] = value;
}


double get_avg(struct Trace_Files * trace_files, int tf) {
	int num_values = sizeof(trace_files->avs[tf]) / sizeof(double);
	int sum = 0;

	for (int i = 0; i < num_values; i++) {
		sum += trace_files->avs[tf][i];
	}

	return (sum / num_values);
}


unsigned int get_value_from_tf(struct Trace_Files * trace_files, int tf, int offset) {
	unsigned char buffer[4];
	FILE * file = trace_files->file_ptrs[tf];

	fseek(file, offset, SEEK_SET);
	if (fread(buffer, 4, 1, file) != 0) {
		unsigned int ret_int = atoi((const char *) buffer);
		return ret_int;
	} else {
		return 0;
	}
}


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

			}
			final_entry += quantum;
		}
		// update avgs
		update_avs(&trace_files, tf, value);
	} while (!done);

	// cleanup
	destroy_page_table(page_table);
	destroy_tlb(tlb);
	destroy_trace_files(trace_files, num_tf);

	// print+format values

}
