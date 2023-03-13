#define _POSIX_C_SOURCE 2
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
  YOUR WORK:

  You will complete the function setup_main_stack below. Missing code
  and calculations are expressed with a questionmark (?). The
  assignment is mostly about doing the correct math. To do that math
  you will need to learn or understand:

  - How the stack is used during program execution
  - How the stack is expected to look like at start of a C-program
  - What a pointer variable is, how it is used
  - How a C-array (pointer to many) is laid-out in memory
  - How pointer arithmetic behaves
  - How to cast from one pointer type to another
  - How to use printf to print good debug information
  - How to use a C-struct, both object and pointer to

  Some manual-pages of interest:

  man -s3 printf
  man -s3 strlen
  man -s3 strncpy
  man -s3 strtok_r

  The prototype for above functions:

  int printf(const char *restrict format, ...);
  size_t strlen(const char *s);
  size_t strncpy(char *dst, const char *src, size_t dstsize);
  char *strtok_r(char *s1, const char *s2, char **lasts);

  The above functions exist in Pintos code. Note the Pintos use
  'strlcpy' instead of 'strncpy'. If 'dst' is large enough they behave
  identical. You can find Pintos implementations in
  /home/TDIU16/labs/skel/pintos/src/lib, see string.c and stdio.c. It
  is strongly recommendable to look at the strtok_r examples given in
  string.c

  About *restrict: http://en.wikipedia.org/wiki/Pointer_alias


  Recommended compile command:

  gcc -m32 -Wall -Wextra -std=gnu99 -g setup-argv.c

*/
#error Read comments above, then remove this line.

#define true 1
#define false 0

typedef int bool;

/* "struct main_args" represent the stack as it must look when
 * entering main. The only issue: argv must point somewhere...
 *
 * The members of this structure is is directly related to the
 * arguments of main in a C-program, of course.
 *
 * int main(int argc, char* argv[]);
 *
 * (char** argv is a more generic form of char* argv[])
 *
 * The function pointer is normally handled by the compiler
 * automatically, and determine the address at where a function shall
 * continue when it returns. The main function does not use it, as the
 * operating system arrange for the program to stop execution when
 * main is finished.
 */
struct main_args
{
  /* Hint: When try to interpret C-declarations, read from right to
   * left! It is often easier to get the correct interpretation,
   * altough it does not always work. */

  /* Variable "ret" that stores address (*ret) to a function taking no
   * parameters (void) and returning nothing. */
  void (*ret)(void);

  /* Just a normal integer. */
  int argc;

  /* Variable "argv" that stores address to an address storing char.
   * That is: argv is a pointer to char*
   */
  char** argv;
};

/* A function that dumps 'size' bytes of memory starting at 'ptr'
 * it will dump the higher adress first letting the stack grow down.
 */
void dump(void* ptr, int size)
{
  int i;
  const int S = sizeof(void*);

  printf("%2$-*1$s \t%3$-*1$s \t%4$-*1$s\n", S*2, "Address", "hex-data", "char-data");

  for (i = size - 1; i >= 0; --i)
  {
    void** adr = (void**)((unsigned long)ptr + i);
    unsigned char* byte = (unsigned char*)((unsigned long)ptr + i);

    printf("%0*lx\t", S*2, (unsigned long)ptr + i); /* address */

    if ((i % S) == 0)
      /* seems we're actually forbidden to read unaligned adresses */
      printf("%0*lx\t", S*2, (unsigned long)*adr); /* content interpreted as address */
    else
      printf("%*c\t", S*2, ' '); /* fill */

    if(*byte >= 32 && *byte < 127)
      printf("%c\n", *byte); /* content interpreted as character */
    else
      printf("\\%o\n", *byte);

    if ((i % S) == 0)
      printf("----------------------------------------------------------------\n");
  }
}

/* Read one line of input ...
 */
void custom_getline(char buf[], int size)
{
  int i;
  for (i = 0; i < (size - 1); ++i)
  {
    buf[i] = getchar();
    if (buf[i] == '\n')
      break;
  }
  buf[i] = '\0';
}

/* Return true if 'c' is fount in the c-string 'd'
 * NOTE: 'd' must be a '\0'-terminated c-string
 */
bool exists_in(char c, const char* d)
{
  int i = 0;
  while (d[i] != '\0' && d[i] != c)
    ++i;
  return (d[i] == c);
}

/* Return the number of words in 'buf'. A word is defined as a
 * sequence of characters not containing any of the characters in
 * 'delimeters'.
 * NOTE: arguments must be '\0'-terminated c-strings
 */
int count_args(const char* buf, const char* delimeters)
{
  int i = 0;
  bool prev_was_delim;
  bool cur_is_delim = true;
  int argc = 0;

  while (buf[i] != '\0')
  {
    prev_was_delim = cur_is_delim;
    cur_is_delim = exists_in(buf[i], delimeters);
    argc += (prev_was_delim && !cur_is_delim);
    ++i;
  }
  return argc;
}

/* Replace calls to STACK_DEBUG with calls to printf. All such calls
 * easily removed later by replacing with nothing. */
#define STACK_DEBUG(...) printf(__VA_ARGS__)

void* setup_main_stack(const char* command_line, void* stack_top)
{
  /* Variable "esp" stores an address, and at the memory loaction
   * pointed out by that address a "struct main_args" is found.
   * That is: "esp" is a pointer to "struct main_args" */
  struct main_args* esp;
  int argc;
  int total_size;
  int line_size;
  int cmdl_size;

  /* "cmd_line_on_stack" and "ptr_save" are variables that each store
   * one address, and at that address (the first) char (of a possible
   * sequence) can be found. */
  char* cmd_line_on_stack;
  char* ptr_save;
  int i = 0;


  /* calculate the bytes needed to store the command_line */
  line_size = YOUR_CODE_HERE;
  STACK_DEBUG("# line_size = %d\n", line_size);

  /* round up to make it even divisible by 4 */
  line_size = YOUR_CODE_HERE;
  STACK_DEBUG("# line_size (aligned) = %d\n", line_size);

  /* calculate how many words the command_line contain */
  argc = YOUR_CODE_HERE;
  STACK_DEBUG("# argc = %d\n", argc);

  /* calculate the size needed on our simulated stack */
  total_size = YOUR_CODE_HERE;
  STACK_DEBUG("# total_size = %d\n", total_size);


  /* calculate where the final stack top will be located */
  esp = YOUR_CODE_HERE;

  /* setup return address and argument count */
  esp->ret = YOUR_CODE_HERE;
  esp->argc = YOUR_CODE_HERE;
  /* calculate where in the memory the argv array starts */
  esp->argv = YOUR_CODE_HERE;

  /* calculate where in the memory the words is stored */
  cmd_line_on_stack = YOUR_CODE_HERE;

  /* copy the command_line to where it should be in the stack */

  /* build argv array and insert null-characters after each word */

  return esp; /* the new stack top */
}

/* The C way to do constants ... */
#define LINE_SIZE 1024

int main()
{
  struct main_args* esp;
  char line[LINE_SIZE];
  void* simulated_stack = malloc(4096);
  void* simulated_stack_top = (void*)((unsigned long)simulated_stack + 4096);
  int i;
  const int S = sizeof(void*);

  /* read one line of input, this will be the command-line */
  printf("Enter a command line: ");
  custom_getline(line, LINE_SIZE);

  /* put initial content on our simulated stack */
  esp = setup_main_stack(line, simulated_stack_top);
  printf("# esp = %0*lx\n", 2*S, (unsigned long)esp);

  if ( (char*)esp >= (char*)simulated_stack_top ||
       (char*)esp < (char*)simulated_stack )
  {
    printf("# ERROR: esp is not inside the allocated stack\n");
    return 1;
  }

  /* dump memory area for verification */
  dump(esp, (char*)simulated_stack_top - (char*)esp);

  /* original command-line should not be needed anymore */
  for (i = 0; i < LINE_SIZE; ++i)
    line[i] = (char)0xCC;

  /* print the argument vector to see if it worked */
  for (i = 0; i < esp->argc; ++i)
  {
    printf("argv[%d] = %s\n", i, esp->argv[i]);
  }
  printf("argv[%d] = %p\n", i, esp->argv[i]);

  free(simulated_stack);

  return 0;
}
