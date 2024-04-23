#include <stddef.h>
#include "threads/synch.h"
#include "plist.h"
#include <stdio.h>


void plist_init(struct p_list *list)
{
    for (int i = 0 ; i<MAX_PLIST ; i++)     //initsierar varje plats i listan
    {
        list->table[i].in_use = false; //platsen används ej just nu
        list->table[i].parent_pid = -1;     //default parent pid
        list->table[i].exit_status = -1;    //default exit status
        list->table[i].alive = false;       
        list->table[i].parent_alive = false;
        list->table[i].waited = false;
        sema_init(&(list->table[i].p_done),0);  //har en semaphore med 0 för att kunna göra viktiga jobb
        lock_init(&list->table[i].lock);
    }
}

int plist_insert(struct p_list* list, int P_PID) //return PID
{
    int pid = -1; //default -1 pid

    for(int i = 1 ; i<MAX_PLIST ; i++)
    {
        if(!list->table[i].in_use)
        {
            lock_acquire(&list->table[i].lock);
            pid = i;
            list->table[i].alive = true;
            list->table[i].in_use = true;
            list->table[i].parent_alive = true;
            list->table[i].parent_pid = P_PID;
            lock_release(&list->table[i].lock);
            return pid;
        }
    }
    return pid;
}

// int plist_find(struct p_list *list, p_info *the_return, int PID) //return PID or -1 if not found
// {
//     int ret_pid = -1;
//     lock_acquire(&list->table[PID].lock);

//     if (list->table[PID].in_use)
//         {
//         the_return = &list->table[PID];
//         ret_pid = PID;
//         }

//     lock_release(&list->table[PID].lock);
//     return ret_pid;
// }


void plist_set_exit_status(p_list* list, int PID, int exit_status)
{
    lock_acquire(&list->table[PID].lock);
    if(list->table[PID].in_use)
    {
        list->table[PID].exit_status = exit_status;
        sema_up(&(list->table[PID].p_done));
    }
    lock_release(&list->table[PID].lock);
}

int plist_get_exit_status(p_list* list, int PID) //returnerar exit status, men -1 om error eller fel PID
{
    sema_down(&(list->table[PID].p_done));
    lock_acquire(&list->table[PID].lock);
    int ret_status = list->table[PID].exit_status;
    list->table[PID].in_use = false; //släpper positionen, alltså kan en ny process skapas här
    lock_release(&list->table[PID].lock);

    return ret_status;
}

int self_status(p_list* list, int pid)
{
    int temp = list->table[pid].exit_status;
    return temp;
}

bool plist_remove(p_list* list, int PID)
{
    bool is_removed = false;
    lock_acquire(&list->table[PID].lock);

    if (list->table[PID].in_use)
    {
        for (int i = 0 ; i < MAX_PLIST ; i++)
            if (list->table[i].in_use && 
                (PID == list->table[i].parent_pid ||
                PID == -1))
                list->table[i].parent_alive = false;

        list->table[PID].alive = false;
        if (!list->table[PID].alive && !list->table[PID].parent_alive)
            list->table[PID].in_use = false;
        
        is_removed = true;
    }
    lock_release(&list->table[PID].lock);
    return is_removed;
}

void plist_clean(p_list* list)
{
/*
    här rensar vi listan, i.e. tar bort alla färdiga funktioner
    som bara ligger och tar upp ett PID värde.

    jag struntar i om världet redan inte används, går bara igenom allt oavsätt.
*/    

    for (int i = 0 ; i < MAX_PLIST ; i++)
        if ((!list->table[i].alive && !list->table[i].parent_alive) || list->table[i].waited)
            list->table[i].in_use = false;
}

//test func
void print_plist(p_list * list)
{
    for(int i = 0;i < MAX_PLIST; i++)
        if(list->table[i].in_use)
            printf("id:%i \tparent_pid:%i Alive:%i parent_alive:%i exit_status:%i waited:%i in_use:%i\n",i, list->table[i].parent_pid, list->table[i].alive, list->table[i].parent_alive, list->table[i].exit_status,list->table[i].waited ,list->table[i].in_use);
}