#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Recommended compile command:
 *
 * gcc -Wall -Wextra -std=gnu99 -pedantic -g main.c memory.c -o main
 *
 * Run your code:
 *
 * ./main
 */
#error Read comments above, then remove this line.


void check_compilation();

int main(void)
{
  char *str = malloc(32);
  str[0] = 'H';
  str[1] = 'e';
  str[2] = 'j';

  printf("String: %p\n", str);
  printf("String: %s\n", str);

  free(str);

  check_compilation();
  return 0;
}
