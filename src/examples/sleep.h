/*

 This file contains some utilities to call "sleep()" only if it exists,
 so that we don't get compilation errors before it is implemented.

 */
#include <stdio.h>
#include <syscall.h>

void sleep(int);

__attribute__((weak))
void sleep(int ms)
{
  (void)ms; /* to avoid warning */
  printf("---------------------\n"
         "You can not run this program before you have implemented\n"
         "the system call sleep()\n"
         "When you have implemented sleep(), you need to re-compile\n"
         "the examples directory (make -C ../examples)\n"
         "---------------------\n");
  exit(999);
}
