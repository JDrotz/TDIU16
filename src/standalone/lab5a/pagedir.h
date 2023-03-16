#ifndef PAGEDIR_H
#define PAGEDIR_H

typedef int bool;
#define false 0
#define true 1

/* Returns true iff 'adr' contains a null-character ('\0'). */
bool is_end_of_string(char* adr);

/* Returns the start address of the page that contains 'adr'. */
void* pg_round_down(const void* adr);

/* Returns the page number of the page that contains 'adr'.
 * Page numbers are counted with start at zero (0).
 */
unsigned pg_no(const void* adr);

/* Uses the page table to translate from logic to physical
 * address. Returns NULL if the translation fail.
 *
 * The first parameter specify the top level page table to
 * use. Normally you will find it in the structure corresponding
 * to your operating systems PCB (Process Control Block).
 */
void *pagedir_get_page (void *pagetable, const void *adr);

/* Page size: The number of bytes in each page. */
#define PGSIZE 100

/* Functions used to simulate a paging system and testing your
 * solution. */
void start_evaluate_algorithm(void* start, int size);
void evaluate(bool result);
void end_evaluate_algorithm();

/* Let us decide how costly a simulated page fault is in terms of
 * time. Does not exist in Pintos. Set to zero (0) for faster
 * testing. */
void simulator_set_pagefault_time(unsigned i);

#endif
