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
	trace_files->avs = (long **) malloc((sizeof(long *)) * num_tf);

	for (int i = 0; i < num_tf; i++) {
		trace_files->avs[i] = (long *) malloc(sizeof(long));
	}

	return trace_files;
}


void destroy_trace_files(struct Trace_Files * trace_files, int num_tf) {
	free(trace_files->file_ptrs);
	free(trace_files->tlbhits);
	free(trace_files->pf);
	free(trace_files->pageout);

	for (int i=0; i < num_tf; i++) {
		free(trace_files->avs[i]);
	}

	free(trace_files->avs);
	free(trace_files);
}


void update_avs(struct Trace_Files * trace_files, int tf, double value) {
	int avs_length = sizeof(trace_files->avs[tf]) / sizeof(long);

	trace_files->avs[tf] = (long *) realloc(trace_files->avs[tf], sizeof(long) * (avs_length+1));  // realloc memory

	trace_files->avs[tf][avs_length+1] = value;  // add new value
}


long get_avg(struct Trace_Files * trace_files, int tf) {
	int num_values = sizeof(trace_files->avs[tf]) / sizeof(long);
	int sum = 0;

	for (int i = 0; i < num_values; i++) {
		sum += trace_files->avs[tf][i];
	}

	return (sum / num_values);
}


unsigned int get_value_from_tf(struct Trace_Files * trace_files, int tf, int index) {
	unsigned char buffer[4];
	size_t offset = index * 4;

	FILE * file = trace_files->file_ptrs[tf];
	//printf("File inside get_value: %p\n", file);
	if (!file) {
		printf("Error while reading from file!\n");
	}

	// if (fseek(file, offset, SEEK_SET) != 0) {
	// 	printf("Unable to seek.\n");
	// }

	fread(buffer, 4, 1, file);
	//buffer[4] = '\0';
	unsigned int ret_int;// = atoi((const char *) buffer);
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
