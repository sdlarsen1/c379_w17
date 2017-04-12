#include <stdlib.h>
#include "page_table.h"
#include <string.h>
#include <stdio.h>

/*
	TO DO:
		complete implementation of PID tracking in add_entry, evict page
*/

// push a new entry into the FIFO array
void FIFO_push(struct Page_Table * pt, int new);
int get_oldest(struct Page_Table * pt);

// update the LRU array
void set_MRU_pt(struct Page_Table * pt, int MRU);
int get_LRU_pt(struct Page_Table * pt);

struct Page_Table * create_page_table(int physpages, char mode) {
    struct Page_Table * page_table;
    page_table = malloc(sizeof(struct Page_Table));

    page_table->num_entries = physpages;

    page_table->logical_addr = malloc((sizeof(int)) * physpages);

    if (mode == 'l') {
        page_table->LRU_table = malloc((sizeof(int)) * physpages);
	memset(page_table->LRU_table, 0, (sizeof (int)) * physpages);
        page_table->FIFO_table = NULL;
    } else {
        page_table->FIFO_table = malloc((sizeof(int)) * physpages);
	memset(page_table->FIFO_table, 0, (sizeof (int)) * physpages);

        page_table->LRU_table = NULL;
    }

    page_table->pid = malloc((sizeof(int)) * physpages);

    // allocate for the valid bit
    page_table->valid = malloc((sizeof(int)) * physpages);
    memset(page_table->valid, 0, (sizeof (int)) * physpages);

    return page_table;
}


void destroy_page_table(struct Page_Table * page_table) {
    free(page_table->logical_addr);
    free(page_table->FIFO_table);
    free(page_table->LRU_table);
    free(page_table->valid);
    free(page_table);
}


int add_entry_pt(struct Page_Table * page_table, int pagenum, char mode, int pid) {
	// check to see if there is a free (invalid)
	int entry, replace, replacement_found = 0, eviction = 0;
	for (entry = 0; entry < page_table->num_entries; entry++)
	{
		if (page_table->valid[entry] == 0)
		{
			replace = entry;
			replacement_found = 1;

			page_table->logical_addr[replace] = pagenum;
			page_table->valid[replace] = 1;

			if (mode == 'f')
				FIFO_push(page_table, replace);

			else if (mode == 'l')
				set_MRU_pt(page_table, replace);

			page_table->pid[replace] = pid;

			break;

		}
	}

	eviction = 0;

	if (!replacement_found)
	{
		// evict a page

		if (mode == 'f')
		{
			eviction = evict_page_FIFO(page_table, pagenum, pid);//
		}

		else if (mode == 'l')
		{
			eviction = evict_page_LRU(page_table, pagenum, pid);
		}
	}

	if (eviction)
		return eviction;

	return 0;
}

// !! potentially return PID of evicted page? !!
int evict_page_LRU(struct Page_Table * page_table, unsigned int new_pagenum, int newpid)
{
	int LRU = get_LRU_pt(page_table);
	int LRU_pid = page_table->pid[LRU];
	page_table->pid[LRU] = newpid;
	page_table->logical_addr[LRU] = new_pagenum;
	set_MRU_pt(page_table, LRU);

	// return PID
	return LRU_pid;
}

int evict_page_FIFO(struct Page_Table * page_table,unsigned int new_pagenum, int newpid)
{
	int oldest = get_oldest(page_table);
	int oldest_pid = page_table->pid[oldest];
	page_table->pid[oldest] = newpid;
	page_table->logical_addr[oldest] = new_pagenum;
	FIFO_push(page_table, oldest);

	// return PID
	return oldest_pid;
}

int query_page_table(struct Page_Table * page_table, unsigned int pagenum, int pid)
{
	int entry;
        for (entry = 0; entry < page_table->num_entries; entry++)
	{
		if ((page_table->valid[entry]) && (page_table->pid[entry] == pid))
			if (page_table->logical_addr[entry] == pagenum)
				return 1;
	}
	return 0;
}

// maybe cache these values in an other array?
double count_entries(struct Page_Table * page_table, int pid)
{
	int entry;
	double count = 0;
        for (entry = 0; entry < page_table->num_entries; entry++)
	{
		if (page_table->pid[entry] == pid)
			if (page_table->valid[entry] == 1)
				count++;
	}

	return count;
}

// add a new page in FIFO, at index {new}.
void FIFO_push(struct Page_Table * pt, int new)
{
	int entry;
        for (entry = 0; entry < pt->num_entries; entry++)
        {
		if (pt->FIFO_table[entry] > 0)
		{
			pt->FIFO_table[entry]++;
		}
	}

	pt->FIFO_table[new] = 1;
}

// return the index of the oldest page
int get_oldest(struct Page_Table * pt)
{
	int entry, oldest = 0, max = 0;
	for (entry = 0; entry < pt->num_entries; entry++)
	{
		if (pt->FIFO_table[entry] > max)
		{
			oldest = entry;
			max = pt->FIFO_table[entry];
		}
	}

	return oldest;
}

// update the LRU array
void set_MRU_pt(struct Page_Table * pt, int MRU)
{
        int MRU_rank = pt->LRU_table[MRU];
        int entry;
        for (entry = 0; entry < pt->num_entries; entry++)
        {
                if (pt->LRU_table[entry] <= MRU_rank)
                {
                        pt->LRU_table[entry]++;
                }
        }

        pt->LRU_table[MRU] = 0;
}

// return the index of the LRU page
int get_LRU_pt(struct Page_Table * pt)
{
        int LRU, max = 0, entry;
        for (entry = 0; entry < pt->num_entries; entry++)
        {
                if (pt->LRU_table[entry] > max)
                {
			max = pt->LRU_table[entry];
                        LRU = entry;
                }
        }

        return LRU;
}


void print_pt(struct Page_Table * pt)
{
        int entry;

	printf("VALID	ADDR	LRUFIFO	PID\n");
        for (entry = 0; entry < pt->num_entries; entry++)
        {
                printf("%d\t%x\t", pt->valid[entry], pt->logical_addr[entry]);
		if (pt->FIFO_table)
			printf("%d\t", pt->FIFO_table[entry]);

		else if (pt->LRU_table)
			printf("%d\t", pt->LRU_table[entry]);

		printf("%d\n", pt->pid[entry]);
        }

}
