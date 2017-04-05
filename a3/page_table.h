#ifndef PAGE_TABLE_H
#define MAX_PHYSPAGES 1000000

struct page_table {
	unsigned int * logical_addr;  // address in logical memory
	unsigned int * physical_addr;  // address in physical memory
}

struct page_table * create_page_table(int physpages);
void destroy_page_table(struct page_table page_table);

void evict_page_LRU(struct page_table page_table);
void evict_page_FIFO(struct page_table page_table);

void add_page(struct page_table page_table);

#endif
