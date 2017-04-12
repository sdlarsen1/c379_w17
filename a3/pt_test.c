#include "page_table.h"
#include <stdio.h>

int main(void)
{
	int test;
	char mode = 'f';
	struct Page_Table * pt = create_page_table(8, mode);
	printf("size: %d\n", pt->num_entries);
	printf("Adding 8 pages\n");
	add_entry_pt(pt, 0xFFF1, mode, 1);
	add_entry_pt(pt, 0xFFF2, mode, 2);
	add_entry_pt(pt, 0xFFF3, mode, 3);
	add_entry_pt(pt, 0xFFF4, mode, 4);
	add_entry_pt(pt, 0xFFF5, mode, 1);
	add_entry_pt(pt, 0xFFF6, mode, 2);
	add_entry_pt(pt, 0xFFF7, mode, 3);
	add_entry_pt(pt, 0xFFF8, mode, 4);

	print_pt(pt);

	printf("Adding 1 more page\n");
	add_entry_pt(pt, 0xCFF2, mode, 2);
	print_pt(pt);

	printf("Querying page 0xFFF4, process 4\n");
        test = query_page_table(pt, 0xFFF4, 4);
        if (test)
                printf("The page was found\n");
        else
                printf("The page was not found\n");

        print_pt(pt);

	printf("Adding 1 more page\n");
        add_entry_pt(pt, 0xFADF, mode,1);
        print_pt(pt);


	printf("Querying page 0xFADF, process 1\n");
        test = query_page_table(pt, 0xFADF, 2);
	if (test)
                printf("The page was found\n");
        else
                printf("The page was not found\n");

	destroy_page_table(pt);


	return 0;
}
