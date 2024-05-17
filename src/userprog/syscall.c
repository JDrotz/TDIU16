#include <stdio.h>
#include <stdlib.h>
#include <syscall-nr.h>
#include "userprog/syscall.h"
#include "threads/interrupt.h"
#include "threads/thread.h"

/* header files you probably need, they are not used yet */
#include <string.h>
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "threads/vaddr.h"
#include "threads/init.h"
#include "userprog/pagedir.h"
#include "userprog/process.h"
#include "devices/input.h"
#include "devices/timer.h"

static void syscall_handler(struct intr_frame *);

static bool verify_variable_length(char* start);
static bool verify_fix_length(void* start, unsigned length);

void syscall_init(void)
{
  intr_register_int(0x30, 3, INTR_ON, syscall_handler, "syscall");
}



bool verify_variable_length(char* start)
{
  if (start == NULL)
    return false;

  if (is_kernel_vaddr(start))
    return false;

//kollar om start är valid
  if(pagedir_get_page(thread_current()->pagedir, start) == NULL)
    return false;
 
  char *cur = start;
  unsigned prev_pg = pg_no(cur);

//kollar allt i adressen och upp till \0 tecken
  while(*cur != '\0')
  {
    prev_pg = pg_no(cur++);
    if(pg_no(cur) != prev_pg) //för att förhindra dupliceringar då pagedir_get_page är dyrt
      if(pagedir_get_page(thread_current()->pagedir, cur) == NULL)
        return false;
  }

  return true;
}

bool verify_fix_length(void* start, unsigned length)
{
  if (start == NULL)
    return false;

  if (is_kernel_vaddr(start))
    return false;

  char* temp = (char*)start;
//loopar över alla adresser från start till length (-1), för att kunna validera dom
//är någon av dom NULL, returnerar vi false
  for (unsigned cur_page = pg_no(start); cur_page <= pg_no((void *)((int)start + length -1)); ++cur_page)
  {
    if (is_kernel_vaddr((void*)temp))
      return false;
    if (pagedir_get_page(thread_current()->pagedir, (void *)(cur_page * PGSIZE)) == NULL)
      return false;
    temp += PGSIZE;
  }
  return true;
}


/* This array defined the number of arguments each syscall expects.
   For example, if you want to find out the number of arguments for
   the read system call you shall write:

   int sys_read_arg_count = argc[ SYS_READ ];

   All system calls have a name such as SYS_READ defined as an enum
   type, see `lib/syscall-nr.h'. Use them instead of numbers.
 */
const int argc[] = {
    /* basic calls */
    0, 1, 1, 1, 2, 1, 1, 1, 3, 3, 2, 1, 1,
    /* not implemented */
    2, 1, 1, 1, 2, 1, 1,
    /* extended, you may need to change the order of these two (plist, sleep) */
    0, 1};

void sys_exit(int status)
{
  process_exit(status);
  thread_exit();
}

static void
syscall_handler(struct intr_frame *f)
{
  int32_t *esp = (int32_t *)f->esp;

  //verifierar från start till \0 som i standalone
  if(!verify_fix_length((void*)esp,4))
    sys_exit(-1);

//fel antal calls (stack fel)
  if(esp[0] > SYS_NUMBER_OF_CALLS)
    sys_exit(-1);
  
  //verifierar från start till slut (start+length-1)
  if(!verify_fix_length(esp, sizeof(char*) * (argc[esp[0]] + 1)))
    sys_exit(-1);

  switch (esp[0] /* retrive syscall number */)
  {
  case SYS_HALT:
  {
    power_off();
    break;
  }

  case SYS_EXIT:
  {
    sys_exit(esp[1]);
    break;
  }

  case SYS_CREATE:
  {
    // 1an är filnamn och 2an är fil storlek enl. anrop
    // (denna returnar alltså en bool från filesys_create)

    //om namn är null eller verify returnerar falskt
    if ((char*)esp[1] == NULL || !verify_fix_length((char*)esp[1],esp[2]))
    {
      sys_exit(-1);
      break;
    }

    f->eax = filesys_create((char *)esp[1], esp[2]);
    break;
  }

  case SYS_OPEN:
  {
    char *file_name = (char *)esp[1];


    if(file_name == NULL || !verify_variable_length(file_name))
    {
      sys_exit(-1);
      break;
    }

    if (file_name == NULL)
    {
      sys_exit(-1);
      break;
    }

    struct file *file = filesys_open(file_name);

    if (file == NULL)
    {
      f->eax = -1;
      break;
    }

    int fd = map_insert(&(thread_current()->open_file_table), file);
    if (fd == -1)
      filesys_close(file);
    //printf("%d\n", fd);
    f->eax = fd;

    break;
  }

  case SYS_REMOVE:
  {
    // returnerar likt create en bool om den lyckats
    f->eax = filesys_remove((char *)esp[1]);
    break;
  }

  case SYS_CLOSE:
  {
    int fd = esp[1];
    value_t file = map_find(&(thread_current()->open_file_table), fd);
    if (file == NULL)
    {
      break;
    }
    filesys_close(file);
    map_remove(&(thread_current()->open_file_table), fd);
    break;
  }

  case SYS_FILESIZE:
  {
    struct file *fil = (struct file *)map_find(&(thread_current()->open_file_table), esp[1]);
    if (fil == NULL)
    {
      f->eax = -1;
      break;
    }

    f->eax = file_length(fil);
    break;
  }

  case SYS_SEEK:
  {
    struct file *file = (struct file *)map_find(&(thread_current()->open_file_table), esp[1]);
    int offset = (int)esp[2];

    if (file != NULL && file_length(file) < offset)
      file_seek(file, file_length(file));
    else if (file != NULL)
      file_seek(file, offset);
    else
    {
      f->eax = -1;
    }
    f->eax = esp[1];
    break;
  }

  case SYS_TELL:
  {
    struct file *file = (struct file *)map_find(&(thread_current()->open_file_table), esp[1]);
    if (file == NULL)
    {
      f->eax = -1;
      break;
    }

    f->eax = file_tell(file);
    break;
  }

  case SYS_READ: // 8
  {
    int fd = esp[1];
    char *buf = (char *)esp[2];
    int length = esp[3];
    int read = 0;

    if (!verify_fix_length(buf, length))
      {
        sys_exit(-1);
        break;
      }

    if (fd == STDOUT_FILENO || buf == NULL)
    {
      f->eax = -1;
      break;
    }
    else if (fd == STDIN_FILENO)
    {

      for (int i = 0; i < length; i++)
      {
        buf[i] = input_getc();

        if (buf[i] == '\r')
          buf[i] = '\n';
        putbuf(&buf[i], 1);
        read++;
      }
      if (read == length)
        f->eax = length;
      else
        f->eax = -1;
      break;
    }
    else // to file
    {
      struct file *temp = map_find(&(thread_current()->open_file_table), fd);
      if (temp == NULL)
      {
        f->eax = -1;
        break;
      }
      f->eax = file_read(temp, buf, length);
    }
    break;
  }

  case SYS_WRITE:
  {
    int fd = esp[1];
    char *buffer = (char *)esp[2];
    int length = esp[3];

    if (!verify_fix_length(buffer, length))
    {
      sys_exit(-1);
      break;
    }

    if (buffer == NULL || fd == STDIN_FILENO)
    {
      f->eax = -1;
      return;
    }

    // om utskrift till skrärm
    if (fd == STDOUT_FILENO)
    {
      putbuf(buffer, esp[3]);
      f->eax = length;
    }

    else // to file
    {

      struct file *temp = (struct file *)map_find(&thread_current()->open_file_table, fd);
      if (temp == NULL || buffer == NULL)
      {
        f->eax = -1;
        return;
      }
      f->eax = file_write(temp, (void *)buffer, length);
    }
    return;
  }
  
  case SYS_EXEC:
  {
    if (!verify_variable_length((char*)esp[1]))
      thread_exit();

    f->eax = process_execute((char*)esp[1]);
    break;
  }

  case SYS_SLEEP:
  {
    timer_msleep((int)esp[1]);
    break;
  }

  case SYS_PLIST:
  {
    process_print_list(); //kallar funktionen i plist. gör all implementering där!!!
    break;
  }
  case SYS_WAIT:
  {
    f->eax = process_wait(esp[1]);
    break;
  }


  default:
  {
    printf("Executed an unknown system call!\n");

    printf("Stack top + 0: %d\n", esp[0]);
    printf("Stack top + 1: %d\n", esp[1]);

    thread_exit();
  }
  }
}
