#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

/* Pintos contain an implementation of a doubly linked list. It is
 * found in ../../lib/kernel/list.[ch].
 *
 * Recommended compilation commands:
 *
 * This is done once:
 *  gcc -Wall -Wextra -std=gnu99 -pedantic -g -c ../../lib/kernel/list.c
 *
 * Then use the precomplied list.o:
 *  gcc -Wall -Wextra -std=gnu99 -pedantic -g use-list.c list.o
 */
#error Read comments above, then remove this line.

#include "../../lib/kernel/list.h"

int main()
{
  int i, j;
  int N = 1000;

  /* create a list */
  for (i = 2; i < N; ++i)
  {
    /* insert the value of the loop variable 'i' in the list */
  }

  for (i = 2; i < N; ++i)
  {
    for (j = i*2; j < N; j += i)
    {
      /* remove every with 'j' even divisible number from the list */
    }
  }

  /* print all remaining numbers in the list and free any memory
   * allocated */
  return 0;
}
