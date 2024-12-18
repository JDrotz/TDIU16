#include <debug.h>
#include <stdio.h>
#include <string.h>

#include "userprog/gdt.h" /* SEL_* constants */
#include "userprog/process.h"
#include "userprog/load.h"
#include "userprog/pagedir.h" /* pagedir_activate etc. */
#include "userprog/tss.h"     /* tss_update */
#include "filesys/file.h"
#include "threads/flags.h" /* FLAG_* constants */
#include "threads/thread.h"
#include "threads/vaddr.h"     /* PHYS_BASE */
#include "threads/interrupt.h" /* if_ */
#include "threads/init.h"      /* power_off() */

/* Headers not yet used that you may need for various reasons. */
#include "threads/synch.h"
#include "threads/malloc.h"
#include "lib/kernel/list.h"
#include "filesys/filesys.h"

#include "userprog/flist.h"
#include "userprog/plist.h"

/* HACK defines code you must remove and implement in a proper way */
#define HACK

struct p_list process_table;

/* This function is called at boot time (threads/init.c) to initialize
 * the process subsystem. */
void process_init(void)
{
   plist_init(&process_table);
}

/* This function is currently never called. As thread_exit does not
 * have an exit status parameter, this could be used to handle that
 * instead. Note however that all cleanup after a process must be done
 * in process_cleanup, and that process_cleanup are already called
 * from thread_exit - do not call cleanup twice! */
void process_exit(int status UNUSED)
{
   plist_set_exit_status(&process_table, thread_current()->tid, status);
}

/* Print a list of all running processes. The list shall include all
 * relevant debug information in a clean, readable format. */
void process_print_list()
{
}

struct parameters_to_start_process
{
   char *command_line;
   struct semaphore sem_ID;
   bool process_status;
   int process_id;
   int parent_id;
};

static void
start_process(struct parameters_to_start_process *parameters) NO_RETURN;

/* Starts a new proccess by creating a new thread to run it. The
   process is loaded from the file specified in the COMMAND_LINE and
   started with the arguments on the COMMAND_LINE. The new thread may
   be scheduled (and may even exit) before process_execute() returns.
   Returns the new process's thread id, or TID_ERROR if the thread
   cannot be created. */
int process_execute(const char *command_line)
{
   char debug_name[64];
   int command_line_size = strlen(command_line) + 1;
   tid_t thread_id = -1;
   int process_id = -1;

   /* LOCAL variable will cease existence when function return! */
   struct parameters_to_start_process arguments;
   int id = thread_current()->tid;
   arguments.parent_id = id; //Ger processen en förälder. 
                              //Används senare i process table också

   debug("%s#%d: process_execute(\"%s\") ENTERED\n",
         thread_current()->name,
         thread_current()->tid,
         command_line);

   /* COPY command line out of parent process memory */
   arguments.command_line = malloc(command_line_size);
   strlcpy(arguments.command_line, command_line, command_line_size);

   strlcpy_first_word(debug_name, command_line, 64);

   sema_init(&(arguments.sem_ID),0);
   arguments.process_id = -1;

   /* SCHEDULES function `start_process' to run (LATER) */
   thread_id = thread_create(debug_name, PRI_DEFAULT,
                             (thread_func *)start_process, &arguments);

   if(thread_id != TID_ERROR)
   {
      sema_down(&(arguments.sem_ID));
      if (thread_id == -1 || !arguments.process_status)  //om thread inte skapas sätter vi -1 
      {
         //returnerar process id -1
         process_id = -1;
      }    
      else //väntar vi på process start, och sätter process id
         process_id = arguments.process_id;

   /* AVOID bad stuff by turning off. YOU will fix this! */
   //power_off();

   /* WHICH thread may still be using this right now? */
   free(arguments.command_line);

   debug("%s#%d: process_execute(\"%s\") RETURNS %d\n",
         thread_current()->name,
         thread_current()->tid,
         command_line, process_id);
   }

   /* MUST be -1 if `load' in `start_process' return false */
   return process_id;
}

/* ASM version of the code to set up the main stack. */
void *setup_main_stack_asm(const char *command_line, void *esp);

/* A thread function that loads a user process and starts it
   running. */
static void
start_process(struct parameters_to_start_process *parameters)
{
   /* The last argument passed to thread_create is received here... */
   struct intr_frame if_;
   bool success;

   char file_name[64];
   strlcpy_first_word(file_name, parameters->command_line, 64);
   

   debug("%s#%d: start_process(\"%s\") ENTERED\n",
         thread_current()->name,
         thread_current()->tid,
         parameters->command_line);

   /* Initialize interrupt frame and load executable. */
   memset(&if_, 0, sizeof if_);
   if_.gs = if_.fs = if_.es = if_.ds = if_.ss = SEL_UDSEG;
   if_.cs = SEL_UCSEG;
   if_.eflags = FLAG_IF | FLAG_MBS;

   success = load(file_name, &if_.eip, &if_.esp);

   debug("%s#%d: start_process(...): load returned %d\n",
         thread_current()->name,
         thread_current()->tid,
         success);

   if (success)
   {
      //skapar den nya processen här. ger den parent pid
      //returnerar sedan unikt PID till parameter listan
      
      thread_current()->tid = plist_insert(&process_table, parameters->parent_id);
      if (thread_current()->tid == -1)
         success = false;
      

      /* We managed to load the new program to a process, and have
         allocated memory for a process stack. The stack top is in
         if_.esp, now we must prepare and place the arguments to main on
         the stack. */

      /* A temporary solution is to modify the stack pointer to
         "pretend" the arguments are present on the stack. A normal
         C-function expects the stack to contain, in order, the return
         address, the first argument, the second argument etc. */

      // if_.esp -= 12; /* this is a very rudimentary solution */

      /* This uses a "reference" solution in assembler that you
         can replace with C-code if you wish. */
      if_.esp = setup_main_stack_asm(parameters->command_line, if_.esp);

      /* The stack and stack pointer should be setup correct just before
         the process start, so this is the place to dump stack content
         for debug purposes. Disable the dump when it works. */

      //    dump_stack ( PHYS_BASE + 15, PHYS_BASE - if_.esp + 16 );
      parameters->process_id = thread_current()->tid;
      parameters->process_status = success;
   }

   debug("%s#%d: start_process(\"%s\") DONE\n",
         thread_current()->name,
         thread_current()->tid,
         parameters->command_line);
      sema_up(&(parameters->sem_ID));

   /* If load fail, quit. Load may fail for several reasons.
      Some simple examples:
      - File doeas not exist
      - File do not contain a valid program
      - Not enough memory
   */
   if (!success)
   {
      parameters->process_id = -1;
      // sema_up(&(parameters->sem_ID));
      thread_exit();
   }

   /* Start the user process by simulating a return from an interrupt,
      implemented by intr_exit (in threads/intr-stubs.S). Because
      intr_exit takes all of its arguments on the stack in the form of
      a `struct intr_frame', we just point the stack pointer (%esp) to
      our stack frame and jump to it. */
   asm volatile("movl %0, %%esp; jmp intr_exit" : : "g"(&if_) : "memory");
   NOT_REACHED();
}

/* Wait for process `child_id' to die and then return its exit
   status. If it was terminated by the kernel (i.e. killed due to an
   exception), return -1. If `child_id' is invalid or if it was not a
   child of the calling process, or if process_wait() has already been
   successfully called for the given `child_id', return -1
   immediately, without waiting.

   This function will be implemented last, after a communication
   mechanism between parent and child is established. */
int process_wait(int child_id)
{
   int status = -1;
   struct thread *cur = thread_current();

   debug("%s#%d: process_wait(%d) ENTERED\n",
         cur->name, cur->tid, child_id);
   /* Yes! You need to do something good here ! */

   if (child_id > MAX_PLIST || child_id < 0)
      return -1;

   if(process_table.table[child_id].in_use == false)
      return -1;

   if((process_table.table[child_id].parent_pid == cur->tid && 
      !process_table.table[child_id].waited &&
      process_table.table[child_id].in_use) ||
      process_table.table[child_id].parent_pid == 0)
   {
      status = plist_get_exit_status(&(process_table), child_id);
   }

   debug("%s#%d: process_wait(%d) RETURNS %d\n",
         cur->name, cur->tid, child_id, status);

   //print_plist(&process_table);

   return status;
}

/* Free the current process's resources. This function is called
   automatically from thread_exit() to make sure cleanup of any
   process resources is always done. That is correct behaviour. But
   know that thread_exit() is called at many places inside the kernel,
   mostly in case of some unrecoverable error in a thread.

   In such case it may happen that some data is not yet available, or
   initialized. You must make sure that nay data needed IS available
   or initialized to something sane, or else that any such situation
   is detected.
*/


//gör om så att man bara hämtar sin egen exit status. utan exit.
void process_cleanup(void)
{
   struct thread *cur = thread_current();
   uint32_t *pd = cur->pagedir;
   int status = self_status(&process_table, cur->tid);

   debug("%s#%d: process_cleanup() ENTERED\n", cur->name, cur->tid);

   /* Later tests DEPEND on this output to work correct. You will have
    * to find the actual exit status in your process list. It is
    * important to do this printf BEFORE you tell the parent process
    * that you exit.  (Since the parent may be the main() function,
    * that may sometimes poweroff as soon as process_wait() returns,
    * possibly before the printf is completed.)
    */
   printf("%s: exit(%d)\n", thread_name(), status);

   if (process_table.table[cur->tid].in_use)
   {
      if (!process_table.table[cur->tid].parent_alive)
         plist_remove(&process_table, cur->tid);
      else
      {
         process_table.table[cur->tid].alive = false;
         //gå igenom barn och sätt parent_alive = false
         for (int i = 0 ; i < MAX_PLIST ; i++)
            if (process_table.table[i].parent_pid == cur->tid)
               process_table.table[i].parent_alive = false;
      }
   }
   plist_clean(&process_table);
   for (int i = 0; i < MAP_SIZE; i++)
   {
      file_close(map_remove(&(thread_current()->open_file_table), i));
   }

   /* Destroy the current process's page directory and switch back
   to the kernel-only page directory. */
   if (pd != NULL)
   {
      /* Correct ordering here is crucial.  We must set
         cur->pagedir to NULL before switching page directories,
         so that a timer interrupt can't switch back to the
         process page directory.  We must activate the base page
         directory before destroying the process's page
         directory, or our active page directory will be one
         that's been freed (and cleared). */
      cur->pagedir = NULL;
      pagedir_activate(NULL);
      pagedir_destroy(pd);
   }
   debug("%s#%d: process_cleanup() DONE with status %d\n",
         cur->name, cur->tid, status);
   sema_up(&process_table.table[cur->tid].p_done);
}

/* Sets up the CPU for running user code in the current
   thread.
   This function is called on every context switch. */
void process_activate(void)
{
   struct thread *t = thread_current();

   /* Activate thread's page tables. */
   pagedir_activate(t->pagedir);

   /* Set thread's kernel stack for use in processing
      interrupts. */
   tss_update();
}
