#include <stdio.h>
#include <stdlib.h>
#include "trace_files.h"
#include <arpa/inet.h>

struct Trace_Files * create_trace_files(int num_tf) {
	struct Trace_Files * trace_files;

	trace_files = (struct Trace_Files *) malloc(sizeof(struct Trace_Files));
	trace_files->file_ptrs = (FILE **) malloc((sizeof(FILE *)) * (num_tf));
	trace_files->tlbhits = (int *) malloc((sizeof(int)) * num_tf);
	trace_files->pf = (int *) malloc((sizeof(int)) * num_tf);
	trace_files->pageout = (int *) malloc((sizeof(int)) * num_tf);
	trace_files->avs = (long *) malloc((sizeof(long)) * num_tf);

	return trace_files;
}


void destroy_trace_files(struct Trace_Files * trace_files, int num_tf) {
	free(trace_files->file_ptrs);
	free(trace_files->tlbhits);
	free(trace_files->pf);
	free(trace_files->pageout);
	free(trace_files->avs);
	free(trace_files);
}


void update_avs(struct Trace_Files * trace_files, int tf, double value) {
	trace_files->avs[tf] += value;
}


long get_avg(struct Trace_Files * trace_files, int tf, int num_accesses) {

	long avg = trace_files->avs[tf] / num_accesses;

	return avg;
}


unsigned int get_value_from_tf(struct Trace_Files * trace_files, int tf, int index) {
	unsigned char buffer[4];
	size_t offset = index * 4;

	FILE * file = trace_files->file_ptrs[tf];
	if (!file) {
		printf("Error while reading from file!\n");
	}

	fread(buffer, 4, 1, file);
	unsigned int ret_int;
	ret_int = buffer[0];
	ret_int <<= 8;
	ret_int += buffer[1];
	ret_int <<= 8;
	ret_int += buffer[2];
	ret_int <<= 8;
	ret_int += buffer[3];

	ret_int = ntohl(ret_int);

	return ret_int;

}
