#include <stdlib.h>
#include "pagedir.h"
#include "thread.h"

/* verfy_*_lenght are intended to be used in a system call that accept
 * parameters containing suspisious (user mode) adresses. The
 * operating system (executng the system call in kernel mode) must not
 * be fooled into using (reading or writing) addresses not available
 * to the user mode process performing the system call.
 *
 * In pagedir.h you can find some supporting functions that will help
 * you dermining if a logic address can be translated into a physical
 * addrerss using the process pagetable. A single translation is
 * costly. Work out a way to perform as few translations as
 * possible.
 *
 * Recommended compilation command:
 *
 *  gcc -Wall -Wextra -std=gnu99 -pedantic -m32 -g pagedir.o verify_adr.c
 */


/* Verify all addresses from and including 'start' up to but excluding
 * (start+length). */
bool verify_fix_length(void* start, unsigned length)
{
//loopar över alla adresser från start till length (-1), för att kunna validera dom
//är någon av dom NULL, returnerar vi false
  for (unsigned cur_page = pg_no(start); cur_page <= pg_no((void *)((int)start + length -1)); ++cur_page)
    if (pagedir_get_page(thread_current()->pagedir, (void *)(cur_page * PGSIZE)) == NULL)
      return false;
  return true;
}

/* Verify all addresses from and including 'start' up to and including
 * the address first containg a null-character ('\0'). (The way
 * C-strings are stored.)
 */
bool verify_variable_length(char* start)
{

//kollar om start är valid
  if(pagedir_get_page(thread_current()->pagedir, start) == NULL)
    return false;
 
  char *cur = start;
  unsigned prev_pg = pg_no(cur);

//kollar allt i adressen och upp till \0 tecken
  while(!is_end_of_string(cur))
  {
    prev_pg = pg_no(cur++);
    if(pg_no(cur) != prev_pg) //för att förhindra dupliceringar då pagedir_get_page är dyrt
      if(pagedir_get_page(thread_current()->pagedir, cur) == NULL)
        return false;
  }

  return true;
}

/* Definition of test cases. */
struct test_case_t
{
  void* start;
  unsigned length;
};

#define TEST_CASE_COUNT 7

const struct test_case_t test_case[TEST_CASE_COUNT] =
{
  {(void*)100, 200}, /* one full page */
  {(void*)100, 100},
  {(void*)199, 102},
  {(void*)101, 98},
  {(void*)250, 190},
  {(void*)250, 200},
  {(void*)250, 210}
};

/* This main program will evalutate your solution. */
int main(int argc, char* argv[])
{
  int i;
  bool result;

  if ( argc == 2 )
  {
    simulator_set_pagefault_time( atoi(argv[1]) );
  }
  thread_init();

  /* Test the algorithm with a given intervall (a buffer). */
  for (i = 0; i < TEST_CASE_COUNT; ++i)
  {
    start_evaluate_algorithm(test_case[i].start, test_case[i].length);
    result = verify_fix_length(test_case[i].start, test_case[i].length);
    evaluate(result);
    end_evaluate_algorithm();
  }

  /* Test the algorithm with a C-string (start address with
   * terminating null-character).
   */
  for (i = 0; i < TEST_CASE_COUNT; ++i)
  {
    start_evaluate_algorithm(test_case[i].start, test_case[i].length);
    result = verify_variable_length(test_case[i].start);
    evaluate(result);
    end_evaluate_algorithm();
  }
  return 0;
}
