#ifndef PAGE_TABLE_H

struct Page_Table {
	unsigned int num_entries;  // number of entries in page table
	unsigned int * logical_addr;  // address in logical memory
	unsigned int * LRU_table;  // LRU table
	unsigned int * FIFO_table;  // FIFO table
	unsigned int * valid;  // valid 'bit' for each entry
	unsigned int * pid;	// process id
};

struct Page_Table * create_page_table(int physpages, char mode);  // mode == f or l
void destroy_page_table(struct Page_Table * page_table);

// !! potentially return PID of evicted page? !!
int evict_page_LRU(struct Page_Table * page_table, unsigned int new_pagenum, int newpid);
int evict_page_FIFO(struct Page_Table * page_table,unsigned int new_pagenum, int newpid);

int query_page_table(struct Page_Table * page_table, unsigned int pagenum, int pid);  // returns 1 if entry exists, else 0

int add_entry_pt(struct Page_Table * page_table, int pagenum, char mode, int pid);  // returns pid of evicted if there is an eviction, else 0

double count_entries(struct Page_Table * page_table, int pid);

void print_pt(struct Page_Table * pt);

#endif
