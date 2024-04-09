#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "memory.h"

/* Recommended compile command:
 *
 * make debug1
 *
 * Run your code:
 *
 * ./debug1
 *
 * Debug with GDB:
 *
 * gdb -tui ./debug1
 */


int main(void)
{
  int values = 5;

  // Detta är en vanlig array av heltal.
  int *contents = malloc(sizeof(int) * values);

  // Detta är en array av pekare till heltal.
  // Tänk: int *data[];
  int **data = malloc(sizeof(int *) * values);

  // Fyll i data:
  for (int i = 0; i < values; i++) {
    contents[i] = i * 5;
    data[i] = &contents[i];
  }

  // Skriv ut data:
  for (int i = 0; i < values; i++) {
    int *pointer = data[i];
    int value = *pointer;
    printf("At %d: %d\n", i, value);
  }

  free(data);
  free(contents);

  return 0;
}
