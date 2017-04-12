#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tlb.h"

// update the LRU table, so that MRU is the most recently used, and all other members are adjusted accordingly
void most_recently_used(struct TLB * tlb, int MRU)
{
	int entry;
	int MRU_rank = tlb->LRU_table[MRU];

	for (entry = 0; entry < tlb->num_entries; entry++)
	{
		if (tlb->LRU_table[entry] <= MRU_rank)
		{
			tlb->LRU_table[entry]++;
		}
	}

	tlb->LRU_table[MRU] = 0;
}

// return index of the entry which is the least recently used
unsigned int get_LRU(struct TLB * tlb)
{
	int LRU = 0, max = 0, entry;
	for (entry = 0; entry < tlb->num_entries; entry++)
	{
		if (tlb->LRU_table[entry] >= max)
		{
			max = tlb->LRU_table[entry];
			LRU = entry;
		}
	}

	return LRU;
}

// allocates space for the tlb data structure, which is defined in tlb.h
struct TLB * create_tlb(int tlbentries, char mode)
{
	struct TLB * tlb;
	tlb = (struct TLB *) malloc(sizeof(struct TLB));

	tlb->num_entries = tlbentries;

	tlb->page_table = (unsigned int *) malloc((sizeof(unsigned int)) * tlbentries);

	if (mode == 'p')
		tlb->ASID_table = NULL;

	else
		tlb->ASID_table = (unsigned int *) malloc((sizeof(unsigned int)) * tlbentries);

	tlb->LRU_table = (unsigned int *) malloc((sizeof(unsigned int)) * tlbentries);
	memset(tlb->LRU_table, 0, (sizeof(unsigned int)) * tlbentries);

	tlb->valid = (unsigned int *) malloc((sizeof(int)) * tlbentries);
	memset(tlb->valid, 0, (sizeof(unsigned int)) * tlbentries);

	return tlb;
}

// searches through the tlb to find an entry with matching asid and pagenum.
int query_entry_tlb(struct TLB * tlb, unsigned int pagenum, unsigned int asid)
{
	int i;
	for(i = 0; i < tlb->num_entries; i++)
	{
		if ((tlb->page_table[i] == pagenum) && (tlb->valid[i] == 1))
		{
			if (tlb->ASID_table == NULL)
			{
				most_recently_used(tlb, i);
				return 1;	// found the entry

			} else
			{
				if (tlb->ASID_table[i] == asid)
				{
					most_recently_used(tlb, i);
					return 1; // found the entry
				}
			}
		}
	}

	return 0;	// not in tlb
}

// adds and entry to the tlb, and returns the index where it was added.
int add_entry_tlb(struct TLB * tlb, unsigned int pagenum, unsigned int asid)
{
	int replace, entry, found_replacement = 0;
	// is there an unused entry to fill?
	for (entry = 0; entry < tlb->num_entries; entry++)
	{
		if (tlb->valid[entry] == 0)
		{
			replace = entry;
			found_replacement = 1;
			break;
		}
	}

	// else, replace LRU
	if (found_replacement == 0)
	{
		replace = get_LRU(tlb);
	}

	tlb->page_table[replace] = pagenum;
	most_recently_used(tlb, replace);
	tlb->valid[replace] = 1;

	if (tlb->ASID_table != NULL)
	{
		tlb->ASID_table[replace] = asid;
	}

	return replace;
}

// set the valid bit for all entries in the tlb to 0
void flush_tlb(struct TLB * tlb)
{
	int entry;
	for (entry = 0; entry < tlb->num_entries; entry++)
	{
		tlb->valid[entry] = 0;
	}
}

// frees all the space allocated by tlb
void destroy_tlb(struct TLB * tlb)
{
	free(tlb->page_table);
	free(tlb->ASID_table);
	free(tlb->LRU_table);
	free(tlb);
}

// prints the contents of the tlb (for testing)
void print_tlb(struct TLB * tlb)
{
	int entry;
	printf("VALID	PAGE	LRU	ASID\n");

	for (entry = 0; entry < tlb->num_entries; entry++)
	{
		printf("%d\t%x\t%d\t", tlb->valid[entry],
					tlb->page_table[entry],
					tlb->LRU_table[entry]);
		if (tlb->ASID_table != NULL)
			printf("%d\n", tlb->ASID_table[entry]);

		else
			printf("NULL\n");
	}
	printf("\n");
}
