#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "map.h"

/* Recommended compile commmand:
 *
 * - make main
 *
 * or
 *
 * - gcc -Wall -Wextra -std=gnu99 -pedantic -g main.c map.c -o main
 *
 * Run your code:
 *
 * ./main
 *
 * Recommended way to test your solution:
 *
 * valgrind --tool=memcheck ./main
 */
#error Read comments above, then remove this line.


/* Can be used to inform compiler about unused parameters (prevent
 * warning). Useful when a funtion pointer expect a certain set of
 * parameters, but you only need some of them. */
#define UNUSED __attribute__((unused))

/* The code assumes that key_t is `int' and value_t is `char*' */

/* function passed as parameter to map_remove_if in order to free the
 * memory for all inseted values, and return true to remove them from
 * the map */
bool do_free (key_t k UNUSED, value_t v, int aux UNUSED)
{
  free(v);     /*! free memory */
  return true; /*  and remove from collection */
}

/* function to display all values in the map */
void print_all (key_t k UNUSED, value_t v, int aux UNUSED)
{
  printf("%s ", v);
}

/* function to display all values in the map that are less than the
 * aux argument */
void print_less (key_t k UNUSED, value_t v, int aux)
{
  /* atoi converst from sequence of character to integer, it will fail
   * when the characters are letters, check the manpage to see how */
  if ( atoi(v) < aux)
  {
    printf("%s ", v);
  }
}


#define LOOPS 10

char* my_strdup (char* str)
{
  /*! calculate the length and add space for '\0' */
  int len = strlen(str) + 1;
  /*! allocate memory just large enough */
  char* dst = malloc(len);
  /*! copy all characters in src to dst */
  strncpy(dst, str, len);

  return dst; /*(!) return our deep copy of str */
}

int main (void)
{
  struct map container;
  char input_buffer[10];
  char* obj;
  int id;
  int i;

  map_init(&container);

  /* remember to try to insert more values than you map can hold */
  printf("Insert values: ");
  for ( i = 0; i < LOOPS; ++i)
  {
    /* insecure, scanf may overflow the input buffer array *
     * very serious, but we ignore it in this test program */
    scanf("%s", input_buffer);

    /*! allocates a copy of the input and inserts in map */
    obj = my_strdup(input_buffer);
    id = map_insert(&container, obj);
  }

  /* remember to test with invalid keys (like 4711, or -1) */
  for ( i = 0; i < LOOPS; ++i)
  {
    printf("Enter id to find value for: ");
    scanf("%d", &id);

    /*! find the value for a key in the map */
    obj = map_find(&container, id);

    /*! if it was found, display it */
YOUR CODE

    /* since we leave the value in the map we may use it again and
     * should not free the memory */
  }

  /* remember to test with invalid keys (like 4711, or -1) */
  for ( i = 0; i < LOOPS; ++i)
  {
    printf("Enter id to remove value for: ");
    scanf("%d", &id);

    /*! find and remove a value for a key in the map */
    obj = map_remove(&container, id);

    /*! if it was found, display it */
YOUR CODE
    /* since we removed the value from the map we will never use it again and
     * must properly free the memory (if it was allocated) */
  }

  /*! print all strings representing an integer less than N */
  printf("Will now display all values less than N. Choose N: ");
  scanf("%d", &i);
  map_for_each(&container, print_less, i);
  printf("\n");

  /*! free all remaining memory and remove from map */
  map_remove_if(&container, do_free, 0);

  /*! print the contents of the map, to make sure it is now empty */
  printf("Will now display all values. The map should be empty now!\n");
  map_for_each(&container, print_all, 0);
  printf("\n");

  return 0;
}
