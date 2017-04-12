#ifndef TRACE_FILES_H

struct Trace_Files {
	FILE ** file_ptrs;  // array of file ptrs
	int * tlbhits;  // array of number of tlbhits per tf
	int * pf;  // array of number pf per tf
	int * pageout;  // array of number of evictions per tf
    long long * avs;  // sum of valid entries used to produce avg valid pages in memory per tf
};

struct Trace_Files * create_trace_files(int num_tf);

unsigned int get_value_from_tf(struct Trace_Files * trace_files, int file, int offset);

void update_avs(struct Trace_Files * trace_files, int tf, double value);

void destroy_trace_files(struct Trace_Files * trace_files, int num_tf);

long get_avg(struct Trace_Files * trace_files, int tf, int num_accesses);

#endif
