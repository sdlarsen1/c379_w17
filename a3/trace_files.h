#ifndef TRACE_FILES_H

struct Trace_Files {
	FILE ** file_ptrs;
	int * tlbhits;
	int * pf;
	int * pageout;
    double ** avs;
};

struct Trace_Files * create_trace_files(int num_tf);

unsigned int get_value_from_tf(struct Trace_Files * trace_files, int file, int offset);

struct Trace_Files * update_avs(struct Trace_Files * trace_files, int tf, double value);

void destroy_trace_files(struct Trace_Files * trace_files, int num_tf);

double get_avg(struct Trace_Files * trace_files, int tf);

#endif
