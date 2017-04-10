#include "tlb.h"
#include <stdio.h>

int main(void)
{
	int test;
	struct TLB * tlb = create_tlb(8, 'g');
	printf("size: %d\n", tlb->num_entries);
	printf("Adding 8 pages\n");
	add_entry_tlb(tlb, 0xFFF1, 1);
	add_entry_tlb(tlb, 0xFFF2, 2);
	add_entry_tlb(tlb, 0xFFF3, 3);
	add_entry_tlb(tlb, 0xFFF4, 4);
	add_entry_tlb(tlb, 0xFFF5, 1);
	add_entry_tlb(tlb, 0xFFF6, 2);
	add_entry_tlb(tlb, 0xFFF7, 3);
	add_entry_tlb(tlb, 0xFFF8, 4);

	print_tlb(tlb);

	printf("Adding 1 more page\n");
	add_entry_tlb(tlb, 0xFFF9, 1);
	print_tlb(tlb);


	printf("Adding 1 more page\n");
	add_entry_tlb(tlb, 0xFFFA, 2);
	print_tlb(tlb);


	printf("Adding 1 more page\n");
	add_entry_tlb(tlb, 0xFFFB, 3);
	print_tlb(tlb);


	printf("Querying page 0xFFF9, process 1\n");
	test = query_entry_tlb(tlb, 0xFFF9, 1);
	if (test)
		printf("The page was found\n");
	else
		printf("The page was not found\n");

	printf("Querying page 0xFFF9, process 2\n");
	test = query_entry_tlb(tlb, 0xFFF9, 1);
	if (test)
                printf("The page was found\n");
        else
                printf("The page was not found\n");


	printf("Flushing tlb...\n");
        flush_tlb(tlb);
        print_tlb(tlb);


	printf("Querying page 0xFFAA, process 1\n");
	test= query_entry_tlb(tlb, 0xFFF9, 1);	
	if (test)
                printf("The page was found\n");
        else
                printf("The page was not found\n");


	
	printf("Adding page 0xFFAA, process 1\n");
	add_entry_tlb(tlb, 0xFFAA, 3);
	print_tlb(tlb);

	printf("Querying page 0xFFF5, process 1\n");
        test = query_entry_tlb(tlb, 0xFFAA, 1);
	if (test)
                printf("The page was found\n");
        else
                printf("The page was not found\n");



	printf("Flushing tlb...\n");
	flush_tlb(tlb);
	print_tlb(tlb);


	printf("Querying page 0xFFAA, process 1\n");
        test = query_entry_tlb(tlb, 0xFFAA, 1);
	if (test)
                printf("The page was found\n");
        else
                printf("The page was not found\n");

	destroy_tlb(tlb);


	return 0;
}
