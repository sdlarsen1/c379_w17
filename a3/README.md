# CMPUT 379 Assignment 3

## Overview


## Structures & Architecture
The tvm379 program consists of the main .c file and 3 separate component files. Each component file has a corresponding header.

- **tlb.c:**
	- header file (`tlb.h`) defines structure of the TLB
	- the C file contains the functions for updating the TLB based on the LRU algorithm, as well as querying entries and flushing the TLB

- **page_table.c:**
	- header file (`page_table.h`) defines the structure of the simulated page table
	- 



## Compilation + Running Instructions
To compile and run the program, enter:

	$ make
	$ ./tvm379 pgsize tlbentries {g|p} quantum physpages {f|l} trace1 trace2 ... tracen

After completion, the program will have output formatted the same as the following example:

	Trace File: 0 | tlbhits: 108 | pf: 28 | pageout: 0 | avs: 16
