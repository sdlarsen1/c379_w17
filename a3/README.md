# CMPUT 379 Assignment 3 - Hayden Bauder & Stephen Larsen

## Structures & Architecture
The tvm379 program consists of the main `tvm379.c` file, 3 separate component files, and a Makefile. Each component file has a corresponding header.

- **tvm379.c:**
	- this is the main function, does all the I/O operations and creates the `TLB`, `Page_Table`, and `Trace_Files` objects
	- runs the simulation and pulls the references from the given trace files and updates the numerical values of the `Trace_Files` object
	- displays final statistics and calls the cleanup commands

- **tlb.c:**
	- header file (`tlb.h`) defines structure of the `TLB` object which consists of
	- the C file contains the functions for updating the TLB based on the LRU algorithm, as well as querying entries and flushing the TLB

- **page_table.c:**
	- header file (`page_table.h`) defines the structure of the simulated `Page_Table` object
	- the C file contains all the utility functions for the page table including the eviction of entries via the FIFO and LRU algorithms

- **trace_files.c:**
	- header file (`trace_files.h`) defines the structure of the `Trace_Files` object
	- the C file contains all the utility functions for the trace files


## Compilation + Running Instructions
To compile and run the program, enter:

	$ make
	$ ./tvm379 pgsize tlbentries {g|p} quantum physpages {f|l} trace1 trace2 ... tracen

After completion, the program will have output formatted the same as the following example:

	Trace File: 0 | tlbhits: 108 | pf: 28 | pageout: 0 | avs: 16
