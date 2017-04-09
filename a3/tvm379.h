#include <stdio.h>

struct Trace_Files {
	FILE ** file_ptrs;
	int * tlbhits;
	int * pf;
	int * pageout;
};

struct Trace_Files * create_trace_files(int num_tf);

unsigned int get_value_from_tf(struct Trace_Files * trace_files, int file, int offset);
