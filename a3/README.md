# CMPUT 379 Assignment 3

# Requirements

### Commmand Line
- pgsize: page (always power of 2 from, size 16 to 65 536)
- tlbentries: number of simulated TLB entires (power of 2 from 8 to 256)
	- `g` if uniform across process, has identifier to indicate process id
	- `p` to indicate process-specific TLB, TLB entries must be flushed every time there is a context switch
- quantum: num. of memeroy references read at a time from each file
- physpages: size of available memory (up to 1 000 000)
	- `f` FIFO eviction
	- `1` LRU eviction

### Infile Processing
- trace files in binary form, each entry 4 bytes, most significant byte first
	- to be read incrementally
- memory references *quantum* at a time
- context switches every *quantum* memory references
