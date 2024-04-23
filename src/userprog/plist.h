#ifndef _PLIST_H_
#define _PLIST_H_

#include <stdbool.h>
#include <stdlib.h>
#include "threads/synch.h"

#define MAX_PLIST 512

/* Place functions to handle a running process here (process list).

   plist.h : Your function declarations and documentation.
   plist.c : Your implementation.

   The following is strongly recommended:

   - A function that given process inforamtion (up to you to create)
     inserts this in a list of running processes and return an integer
     that can be used to find the information later on.

   - A function that given an integer (obtained from above function)
     FIND the process information in the list. Should return some
     failure code if no process matching the integer is in the list.
     Or, optionally, several functions to access any information of a
     particular process that you currently need.

   - A function that given an integer REMOVE the process information
     from the list. Should only remove the information when no process
     or thread need it anymore, but must guarantee it is always
     removed EVENTUALLY.

   - A function that print the entire content of the list in a nice,
     clean, readable format.

 */
typedef struct p_list p_list;
typedef struct p_info p_info;


struct p_info
{
  bool in_use;
  int parent_pid;
  int exit_status;
  bool alive;
  bool parent_alive;
  bool waited;
  struct semaphore p_done;
  struct lock lock;
};

struct p_list
{
  p_info table [MAX_PLIST];
};


void plist_init(struct p_list *list);

int plist_insert(struct p_list* list, int P_PID); //return PID

// int plist_find(struct p_list *list, p_info *the_return, int PID); //return PID or -1 if not found

void plist_set_exit_status(p_list* list, int PID, int exit_status);

int plist_get_exit_status(p_list* list, int PID);

bool plist_remove(p_list* list, int PID);

void plist_clean(p_list* list);

int self_status(p_list* list, int pid);

//test func
void print_plist(p_list * list);

#endif
