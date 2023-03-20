#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

/* Recommended compile command:
 *
 * make debug2
 *
 * Run your code:
 *
 * ./debug2
 *
 * Debug with GDB:
 *
 * gdb -tui ./debug2
 */


// Skapa en array av tal.
// Notera: denna typ av slump är inte särskilt bra, men
// duger för vårt exempel.
int *create_numbers(int count)
{
  srand(time(NULL));

  int *result = malloc(sizeof(int) * count);

  for (int i = 0; i < count; i++)
    result[i] = rand() % 512;

  return result;
}

// Skriv ut en array av tal.
void print_numbers(int *numbers, int count)
{
  for (int i = 0; i < count; i++) {
    int number = numbers[i];
    printf("Number %d: %d\n", i, number);
  }
}

// Skriv ut tal med en rubrik ovanför.
void print_with_header(const char *header, int *numbers, int count)
{
  printf("%s\n", header);
  printf("------------------\n");

  print_numbers(numbers, count);
  free(numbers);
}

int main(void)
{
  int count = 12;
  int *numbers = create_numbers(count);
  print_with_header("First time:", numbers, count);
  printf("\n");

  print_with_header("Second time:", numbers, count);
  free(numbers);

  return 0;
}
