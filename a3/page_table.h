#ifndef PAGE_TABLE_H
#define MAX_PHYSPAGES 1000000

struct Page_Table {
	unsigned int num_entries;
	unsigned int * logical_addr;  // address in logical memory
	unsigned int * physical_addr;  // address in physical memory
	unsigned int * LRU_table;
	unsigned int * FIFO_table;
	unsigned int * valid;
	unsigned int * pid;	// process id
};

struct Page_Table * create_page_table(int physpages, char mode);  // mode == f or l
void destroy_page_table(struct Page_Table * page_table);

// !! potentially return PID of evicted page? !!
void evict_page_LRU(struct Page_Table * page_table, unsigned int new_pagenum);
void evict_page_FIFO(struct Page_Table * page_table,unsigned int new_pagenum);

int query_page_table(struct Page_Table * page_table, unsigned int pagenum);  // returns 1 if entry exists, else 0

int add_entry_pt(struct Page_Table * page_table, int pagenum, char mode);  // returns 1 if there is an eviction, else 0



#endif
