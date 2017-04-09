#ifndef PAGE_TABLE_H
#define MAX_PHYSPAGES 1000000

struct Page_Table {
	unsigned int * logical_addr;  // address in logical memory
	unsigned int * physical_addr;  // address in physical memory
	unsigned int * LRU_table;
	unsigned int * FIFO_table;
}

struct Page_Table * create_page_table(int physpages, char mode);  // mode == f or 1
void destroy_page_table(struct Page_Table * page_table);

void evict_page_LRU(struct Page_Table * page_table);
void evict_page_FIFO(struct Page_Table * page_table);

int query_page_table(struct Page_Table * page_table, unsigned int pagenum);  // returns 1 if entry exists, else 0

int add_entry_pt(struct Page_Table * page_table, int pagenum, char mode);  // returns 1 if there is an eviction, else 0

#endif
