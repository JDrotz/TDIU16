#include <stdio.h>
/* halt.c

   Simple program to test whether running a user program works.

   Just invokes a system call that shuts down the OS. */

#include <syscall.h>

int main(void)
{
  // halt();
  return 111;
  /* not reached */
  printf("hejhej");
}
