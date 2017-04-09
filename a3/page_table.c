#include <stdlib.c>
#include "page_table.h"


struct Page_Table * create_page_table(int physpages, char mode) {
    struct Page_Table * page_table;
    page_table = malloc(sizeof struct Page_Table);
    page_table->logical_addr = malloc((sizeof int) * physpages);
    page_table->physical_addr = malloc((sizeof int) * physpages);

    if (mode == '1') {
        page_table->LRU_table = malloc((sizeof int) * physpages);
        page_table->FIFO_table = NULL
    } else {
        page_table->FIFO_table = malloc((sizeof int) * physpages);
        page_table->LRU_table = NULL;
    }
}


void destroy_page_table(struct Page_Table * page_table) {
    free(page_table->logical_addr);
    free(page_table->physical_addr);
    free(page_table->FIFO_table);
    free(page_table->LRU_table);
    free(page_table);
}
