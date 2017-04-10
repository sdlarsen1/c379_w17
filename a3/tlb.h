#ifndef TLB_H
#define MAX_TLB_ENTRIES 256
#define MIN_TLB_ENTRIES 8

struct TLB
{
	char mode;	// g or p
	unsigned int num_entries;
	unsigned int * page_table;
	unsigned int * ASID_table;
	unsigned int * LRU_table;
	unsigned int * valid;
};

struct TLB * create_tlb(int tlbentries, char mode);		// mode == g or p

int query_entry_tlb(struct TLB * tlb, unsigned int pagenum, unsigned int asid);	// returns 1 if page in table, else 0

int add_entry_tlb(struct TLB * tlb, unsigned int pagenum, unsigned int asid);
// returns the index of the replaced entry

void destroy_tlb(struct TLB * tlb);

void flush_tlb(struct TLB * tlb);

void print_tlb(struct TLB * tlb);

#endif
