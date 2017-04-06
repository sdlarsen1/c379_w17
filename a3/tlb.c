#include <stdlib.h>
#include "tlb.h"

struct TLB * create_tlb(int tlbentries, char mode)
{
	struct TLB * tlb;
	tlb = malloc(sizeof struct TLB);
	tlb->page_table = malloc((sizeof int) * tlbentries);
	if (mode == 'p')
		tlb->ASID_table = NULL;
	else
		tlb->ASID_table = malloc((sizeof int) * tlbentries);

	tlb->LRU_table = malloc((sizeof int) * tlbentries);

}

int query_entry(struct TLB * tlb, unsigned int pagenum);

void add_entry(struct TLB * tlb, int pagenum, int asid);

void destroy_tlb(struct TLB * tlb);
