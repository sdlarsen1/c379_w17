#ifndef TLB_H
#define MAX_TLB_ENTRIES 256
#define MIN_TLB_ENTRIES 8

struct TLB 
{
	unsigned int * page_table;
	unsigned int * ASID_table;
	unsigned int * LRU_table;
};

struct TLB * create_tlb(int tlbentries, char mode);		// mode == g or p

int query_entry(struct TLB * tlb, unsigned int pagenum);	// returns 1 if page in table, else 0

void add_entry(struct TLB * tlb, int pagenum, int asid);

void destroy_tlb(struct TLB * tlb);

#endif
