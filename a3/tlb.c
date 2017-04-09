#include <stdlib.h>
#include "tlb.h"

// update the LRU table
void most_recently_used(struct TLB * tlb, int MRU)
{
	int MRU_rank = tlb->LRU_table[entry];
	int entry;
	for (entry = 0; entry < tlb->num_entries; entry++)
	{
		if (tlb->LRU_table[entry] <= MRU_rank)
		{
			tlb->LRU_table[entry]++;
		}
	}

	tlb->LRU_table[MRU] = 0;	
}

// return index of entry that is LRU
unsigned int get_LRU(struct TLB * tlb)
{
	int LRU, max = 0, entry;
	for (entry = 0; entry < tlb->num_entries; entry++)
	{
		if (tlb->LRU_table[entry] > max)
		{
			LRU = entry;
		}
	}

	return LRU;
}

struct TLB * create_tlb(int tlbentries, char mode)
{
	struct TLB * tlb;
	tlb = (struct TLB *) malloc(sizeof struct TLB);
	tlb->page_table = (unsigned int *) malloc((sizeof int) * tlbentries);

	if (mode == 'p')
		tlb->ASID_table = NULL;

	else
		tlb->ASID_table = (unsigned int *) malloc((sizeof int) * tlbentries);

	tlb->LRU_table = (unsigned int *) malloc((sizeof int) * tlbentries);
	memset(tlb->LRU_table, 0, (sizeof int) * tlbentries);

}

int query_entry_tlb(struct TLB * tlb, unsigned int pagenum, int asid)
{
	int i;
	for(i = 0; i < tlb->num_entries; i++)
	{
		if (tlb->page_table[i] == pagenum)
		{
			if (tlb->ASID_table == NULL)
				return 1;	// found the entry

			else
			{
				if (tlb->ASID_table[i] == asid)
					return 1; // found the entry
			}
		}
	}

	return 0;	// not in tlb
}

void add_entry_tlb(struct TLB * tlb, unsigned int pagenum, int asid);


void destroy_tlb(struct TLB * tlb)
{
	free(tlb->page_table);
	free(tlb->ASID_table);
	free(tlb->LRU_table);
	free(tlb);
}
